#!/bin/bash

echo "==== Git Auto Commit & Push ===="

# Check if inside a git repo
if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    echo "Error: Not inside a Git repository."
    exit 1
fi

# Show status first (nice for workflow)
echo ""
git status
echo ""

# Ask user for commit message
read -p "Enter commit message: " COMMIT_MSG

# If empty, set default
if [ -z "$COMMIT_MSG" ]; then
    COMMIT_MSG="Auto commit"
fi

# Check if there are changes
if [[ -z $(git status --porcelain) ]]; then
    echo "No changes to commit."
    exit 0
fi

echo "Adding files..."
git add -A

echo "Committing..."
git commit -m "$COMMIT_MSG"

CURRENT_BRANCH=$(git branch --show-current)

echo "Pushing to origin/$CURRENT_BRANCH ..."
git push origin "$CURRENT_BRANCH"

echo "Done 👍"
