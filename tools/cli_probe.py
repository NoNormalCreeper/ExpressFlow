#!/usr/bin/env python3
import argparse
import json
import os
import pexpect
import time
from pathlib import Path

STATE_DIR = Path(".codex_cli_probe")
STATE_DIR.mkdir(exist_ok=True)

def main():
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_start = sub.add_parser("run")
    p_start.add_argument("command", nargs=argparse.REMAINDER)

    p_script = sub.add_parser("script")
    p_script.add_argument("file")

    args = parser.parse_args()

    if args.cmd == "run":
        run_interactive(args.command)
    elif args.cmd == "script":
        run_script(Path(args.file))

def spawn(command):
    env = os.environ.copy()
    env["TERM"] = "xterm-256color"
    env["NO_COLOR"] = "1"

    return pexpect.spawn(
        "bash",
        ["-lc", " ".join(command)],
        encoding="utf-8",
        timeout=2,
        dimensions=(30, 120),
        env=env,
    )

def drain(child, wait=0.2):
    time.sleep(wait)
    chunks = []
    while True:
        try:
            chunks.append(child.read_nonblocking(size=4096, timeout=0.05))
        except pexpect.TIMEOUT:
            break
        except pexpect.EOF:
            break
    return "".join(chunks)

def run_interactive(command):
    child = spawn(command)
    print(drain(child, 0.3), end="")

    while child.isalive():
        try:
            line = input("\n[send]> ")
        except EOFError:
            break

        if line == ":quit":
            child.close(force=True)
            break

        child.sendline(line)
        out = drain(child, 0.2)
        print(out, end="")

def run_script(path):
    spec = json.loads(path.read_text(encoding="utf-8"))
    child = spawn(spec["command"])

    transcript = []
    transcript.append({
        "action": "start",
        "output": drain(child, spec.get("initial_wait", 0.3)),
    })

    for step in spec["steps"]:
        value = step["send"]
        child.sendline(str(value))
        output = drain(child, step.get("wait", 0.2))
        transcript.append({
            "send": value,
            "output": output,
        })

    if child.isalive():
        child.close(force=True)

    print(json.dumps(transcript, ensure_ascii=False, indent=2))

if __name__ == "__main__":
    main()