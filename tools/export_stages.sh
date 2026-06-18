#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
    echo "Usage: $0 [output-dir]" >&2
    exit 2
fi

repo_root="$(git rev-parse --show-toplevel)"
output_dir="${1:-"$repo_root/exported_stages"}"

if [[ -e "$output_dir" ]]; then
    echo "Output directory already exists: $output_dir" >&2
    exit 1
fi

mkdir -p "$output_dir"

export_stage() {
    local stage="$1"
    local branch="$2"
    local commit
    local stage_dir

    commit="$(git -C "$repo_root" rev-parse --verify "$branch^{commit}")"
    stage_dir="$output_dir/$stage"
    mkdir -p "$stage_dir"

    git -C "$repo_root" archive --format=tar "$commit" | tar -x -C "$stage_dir"

    {
        echo "stage=$stage"
        echo "branch=$branch"
        echo "commit=$commit"
        echo "exported_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
    } > "$stage_dir/EXPORT_INFO.txt"
}

export_stage stage1 stage1-basic-system
export_stage stage2 stage2-deliveryman-tasks
export_stage stage3 stage3-socket-cs

cat > "$output_dir/README.txt" <<EOF
ExpressFlow staged export

stage1/ -> stage1-basic-system
stage2/ -> stage2-deliveryman-tasks
stage3/ -> stage3-socket-cs

Each directory is exported from the commit recorded in EXPORT_INFO.txt.
Uncommitted working-tree files are not included.
EOF

echo "Exported stages to: $output_dir"
