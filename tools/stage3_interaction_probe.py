#!/usr/bin/env python3
import argparse
import json
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path


def run_probe(name, command, steps, transcript_dir):
    spec_path = transcript_dir / f"{name}.script.json"
    spec_path.write_text(
        json.dumps(
            {
                "command": command,
                "initial_wait": 0.3,
                "steps": [{"send": step, "wait": 0.15} for step in steps],
            },
            ensure_ascii=False,
            indent=2,
        ),
        encoding="utf-8",
    )
    result = subprocess.run(
        [sys.executable, "tools/cli_probe.py", "script", str(spec_path)],
        check=True,
        text=True,
        capture_output=True,
    )
    transcript_path = transcript_dir / f"{name}.transcript.json"
    transcript_path.write_text(result.stdout, encoding="utf-8")
    return json.loads(result.stdout)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--build-dir", default="cmake-build-debug-clang-libc++")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", default="18186")
    parser.add_argument("--data-dir", default="data_stage3")
    parser.add_argument("--out-dir", default="data_stage3/probe_transcripts")
    args = parser.parse_args()

    build_dir = Path(args.build_dir)
    server = build_dir / "stage3_server"
    client = build_dir / "stage3_client"
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix="expressflow-stage3-probe-") as tmp:
        runtime_data = Path(tmp) / "data"
        shutil.copytree(args.data_dir, runtime_data)

        server_proc = subprocess.Popen(
            [str(server), args.host, args.port, str(runtime_data)],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        try:
            time.sleep(0.4)
            if server_proc.poll() is not None:
                output = server_proc.stdout.read() if server_proc.stdout else ""
                raise RuntimeError("stage3_server exited early:\n" + output)
            command = [str(client), args.host, args.port]
            transcripts = {
                "user_send": run_probe(
                    "user_send",
                    command,
                    [
                        "1",
                        "1",
                        "u1",
                        "123456",
                        "1",
                        "u2",
                        "stage3-auto-box",
                        "1",
                        "1",
                        "5",
                        "2",
                    ],
                    out_dir,
                ),
                "admin_manage": run_probe(
                    "admin_manage",
                    command,
                    [
                        "1",
                        "3",
                        "admin",
                        "admin",
                        "1",
                        "2",
                        "3",
                        "c_auto",
                        "Auto Courier",
                        "13911112222",
                        "123456",
                        "4",
                        "5",
                        "6",
                        "1",
                        "1",
                        "7",
                        "8",
                        "1",
                        "9",
                        "2",
                    ],
                    out_dir,
                ),
                "courier_pickup": run_probe(
                    "courier_pickup",
                    command,
                    ["1", "2", "c1", "123456", "1", "2", "2", "3", "4", "2"],
                    out_dir,
                ),
                "user_sign": run_probe(
                    "user_sign",
                    command,
                    ["1", "1", "u2", "123456", "2", "3", "1", "4", "5", "2"],
                    out_dir,
                ),
            }
            summary_path = out_dir / "stage3_interaction_probe_transcript.json"
            summary_path.write_text(
                json.dumps(transcripts, ensure_ascii=False, indent=2),
                encoding="utf-8",
            )
            print(summary_path)
        finally:
            server_proc.terminate()
            try:
                server_proc.wait(timeout=2)
            except subprocess.TimeoutExpired:
                server_proc.kill()


if __name__ == "__main__":
    main()
