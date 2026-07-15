<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>G E R R I T &nbsp; C O M M A N D S</samp>
</p>

<h1 align="center">📘 Gerrit Code Review — Complete Command Reference</h1>

<p align="center"><em>"Push for review, not to main. That's the Gerrit way."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | What is Gerrit? | [§1](#-1-what-is-gerrit) |
| 2 | Initial Setup | [§2](#-2-initial-setup) |
| 3 | The Gerrit Workflow | [§3](#-3-the-gerrit-workflow) |
| 4 | Pushing Changes for Review | [§4](#-4-pushing-changes-for-review) |
| 5 | Working with Change-Id | [§5](#-5-working-with-change-id) |
| 6 | Amending & Updating Changes | [§6](#-6-amending--updating-changes) |
| 7 | Fetching & Checking Out Changes | [§7](#-7-fetching--checking-out-changes) |
| 8 | Reviewing & Voting | [§8](#-8-reviewing--voting) |
| 9 | Submitting & Merging | [§9](#-9-submitting--merging) |
| 10 | Topic & Relation Chains | [§10](#-10-topic--relation-chains) |
| 11 | Cherry-Pick & Rebase | [§11](#-11-cherry-pick--rebase) |
| 12 | Gerrit SSH Commands | [§12](#-12-gerrit-ssh-commands) |
| 13 | Gerrit REST API (curl) | [§13](#-13-gerrit-rest-api-curl) |
| 14 | Branch & Tag Management | [§14](#-14-branch--tag-management) |
| 15 | Troubleshooting & Tips | [§15](#-15-troubleshooting--tips) |
| 16 | Quick Reference Cheat Sheet | [§16](#-16-quick-reference-cheat-sheet) |

---

## 🔹 1. What is Gerrit?

### The Concept

**Gerrit** is a web-based code review system built on top of Git. Unlike GitHub/GitLab where you push to a branch and open a Pull Request, in Gerrit you push to a **special review ref** and the system creates a **Change** for review.

```
┌──────────────────────────────────────────────────────────┐
│                  Standard Git Flow                       │
│  Developer → push to branch → merge directly             │
├──────────────────────────────────────────────────────────┤
│                  Gerrit Flow                             │
│  Developer → push to refs/for/branch → Review → Submit   │
│              ↑                          ↑        ↑       │
│         Creates a Change         Reviewers    Merge      │
│         with unique Change-Id    vote +1/+2  to branch   │
└──────────────────────────────────────────────────────────┘
```

### Key Terminology

| Term | Meaning |
|------|---------|
| **Change** | A single code review unit. Like a Pull Request but for one commit. |
| **Patch Set** | A version of a Change. Amending and re-pushing creates a new Patch Set. |
| **Change-Id** | A unique identifier (`I` + 40 hex chars) in the commit message that ties commits to Changes. |
| **Code-Review** | A vote label. Typically `-2` to `+2`. `+2` means "approved to merge." |
| **Verified** | A vote label. Usually set by CI/CD. `+1` = build passed. |
| **Submit** | Merging a Change into the target branch after approval. |
| **Abandon** | Closing a Change without merging (like closing a PR). |
| **refs/for/** | The magic Git ref that tells Gerrit "create a review." |
| **refs/changes/** | Where Gerrit stores each Change and its Patch Sets. |

---

## 🔹 2. Initial Setup

### Cloning a Gerrit Repository

```bash
# Clone using the Gerrit URL
git clone ssh://username@gerrit.example.com:29418/project/repo.git

# Or with HTTP
git clone https://gerrit.example.com/a/project/repo.git
```

### Installing the Change-Id Hook

This is **essential**. The commit-msg hook automatically adds a `Change-Id` line to every commit message.

```bash
# Navigate into your cloned repo
cd repo

# Install the hook (SSH)
scp -p -P 29418 username@gerrit.example.com:hooks/commit-msg .git/hooks/

# Or download via HTTP
curl -Lo .git/hooks/commit-msg https://gerrit.example.com/tools/hooks/commit-msg
chmod +x .git/hooks/commit-msg
```

### Verify the Hook Works

```bash
git commit --allow-empty -m "Test commit"
git log -1
# You should see a line like:
#   Change-Id: I1a2b3c4d5e6f7890abcdef1234567890abcdef12
```

### Configure Git for Gerrit

```bash
# Set your Gerrit username
git config user.name "Your Name"
git config user.email "your.email@company.com"

# Optional: Set a push shortcut
git config remote.origin.push refs/heads/*:refs/for/*
```

> 💡 **Tip:** With the push shortcut above, a plain `git push` automatically goes to `refs/for/<branch>` instead of directly to the branch.

---

## 🔹 3. The Gerrit Workflow

### The Standard Development Cycle

```
  ┌─── 1. Create branch ───────────────────────────────────────┐
  │      git checkout -b my-feature origin/master               │
  │                                                             │
  ├─── 2. Make changes & commit ────────────────────────────────┤
  │      git add -A && git commit                               │
  │      (hook adds Change-Id automatically)                    │
  │                                                             │
  ├─── 3. Push for review ─────────────────────────────────────┤
  │      git push origin HEAD:refs/for/master                   │
  │      → Gerrit creates Change #12345                         │
  │                                                             │
  ├─── 4. Address review feedback ──────────────────────────────┤
  │      (edit files)                                           │
  │      git add -A && git commit --amend                       │
  │      git push origin HEAD:refs/for/master                   │
  │      → Gerrit updates Change #12345 (Patch Set 2)           │
  │                                                             │
  ├─── 5. Reviewer approves (+2) ──────────────────────────────┤
  │                                                             │
  └─── 6. Submit (merge) ──────────────────────────────────────┘
```

### Golden Rules

1. **One commit = one Change.** Don't squash unrelated work into one commit.
2. **Never** push directly to `master` / `main`. Always push to `refs/for/<branch>`.
3. **Amend** your commit to update a review — don't create new commits (unless they're separate Changes).
4. **Keep the Change-Id** in your commit message. It's what links pushes to reviews.

---

## 🔹 4. Pushing Changes for Review

### Basic Push

```bash
# Push current branch to review on master
git push origin HEAD:refs/for/master
```

### Push to a Specific Branch

```bash
# Push for review on the 'develop' branch
git push origin HEAD:refs/for/develop

# Push for review on a release branch
git push origin HEAD:refs/for/release/2.0
```

### Push with Review Options (Push Options)

```bash
# Assign reviewers
git push origin HEAD:refs/for/master%r=alice@company.com,r=bob@company.com

# Add CC
git push origin HEAD:refs/for/master%cc=team-lead@company.com

# Set topic
git push origin HEAD:refs/for/master%topic=bugfix-login

# Set label (e.g., mark as Work-In-Progress)
git push origin HEAD:refs/for/master%wip

# Mark as Ready for review (remove WIP)
git push origin HEAD:refs/for/master%ready

# Set a hashtag
git push origin HEAD:refs/for/master%hashtag=sprint-42

# Combine multiple options
git push origin HEAD:refs/for/master%r=alice@company.com,topic=my-feature,hashtag=sprint-42
```

### Push Using `-o` Options (Gerrit 3.x+)

```bash
# Add reviewer
git push origin HEAD:refs/for/master -o r=alice@company.com

# Set topic
git push origin HEAD:refs/for/master -o topic=my-feature

# Mark as WIP
git push origin HEAD:refs/for/master -o wip

# Mark as private
git push origin HEAD:refs/for/master -o private

# Remove private flag
git push origin HEAD:refs/for/master -o remove-private

# Multiple options
git push origin HEAD:refs/for/master -o r=alice@company.com -o topic=bugfix -o l=Verified+1
```

### Push a Draft Change (Gerrit 2.x — deprecated in 3.x)

```bash
# Old-style draft (Gerrit 2.x)
git push origin HEAD:refs/drafts/master

# Modern equivalent: use WIP
git push origin HEAD:refs/for/master%wip
```

---

## 🔹 5. Working with Change-Id

### The Concept

The **Change-Id** is the bridge between your local commit and the Gerrit Change. It's a line in your commit message that looks like:

```
Change-Id: Iab3f5e29c7d41b8fd98ab301c77d5a4f3e8cd902
```

Without it, every push creates a **new** Change. With it, pushes update the **existing** Change.

### How It's Generated

```
commit-msg hook runs after you write your message
        ↓
Computes a hash from: tree, parent, author, committer, message
        ↓
Prepends 'I' → Change-Id: I<40-hex-chars>
        ↓
Appends it to end of commit message
```

### Manually Adding a Change-Id (if hook wasn't installed)

```bash
# Generate one manually
echo "Change-Id: I$(git log -1 --format='%H')"

# Or install the hook retroactively and amend
scp -p -P 29418 username@gerrit.example.com:hooks/commit-msg .git/hooks/
git commit --amend --no-edit
```

### What Happens Without a Change-Id

```bash
git push origin HEAD:refs/for/master
# ERROR: missing Change-Id in message footer

# Fix: amend to add it (if hook is installed)
git commit --amend --no-edit
git push origin HEAD:refs/for/master
```

> ⚠️ **Warning:** The Change-Id must be on the **last line** of the commit message (or last in the footer block). Any text after it will break things.

---

## 🔹 6. Amending & Updating Changes

### The Concept

In Gerrit, you don't create follow-up commits for review feedback. You **amend** the original commit, keeping the same Change-Id, and push again. Gerrit sees the same Change-Id and creates a new **Patch Set** under the same Change.

### Update a Change (Most Common Workflow)

```bash
# 1. Make your code changes
vim src/main.cpp

# 2. Stage the changes
git add src/main.cpp

# 3. Amend the existing commit (KEEP the Change-Id!)
git commit --amend

# 4. Push again — Gerrit sees the same Change-Id → new Patch Set
git push origin HEAD:refs/for/master
```

### 🖥️ Output

```
remote: Processing changes: refs: 1, done
remote:
remote: SUCCESS
remote:
remote:   https://gerrit.example.com/c/project/+/12345 Subject of change [UPDATED]
```

### Amend Without Changing the Message

```bash
git commit --amend --no-edit
```

### Interactive Rebase to Edit an Older Commit

If the Change you need to update isn't the latest commit:

```bash
# See your commit history
git log --oneline -5

# Output:
# abc1234 Third change      ← HEAD
# def5678 Second change     ← This is the one to edit
# 789abcd First change

# Start interactive rebase
git rebase -i HEAD~3

# In the editor, change 'pick' to 'edit' for the commit you want to modify:
#   pick 789abcd First change
#   edit def5678 Second change     ← Changed to 'edit'
#   pick abc1234 Third change

# Git stops at that commit. Make your changes:
vim file.cpp
git add file.cpp
git commit --amend --no-edit

# Continue the rebase
git rebase --continue

# Push all updated changes
git push origin HEAD:refs/for/master
```

> 💡 **Tip:** Each commit in the chain has its own Change-Id, so each one gets updated as a separate Gerrit Change.

---

## 🔹 7. Fetching & Checking Out Changes

### The Concept

Need to review someone's code locally, or resume work on a Change? You can fetch any Change from Gerrit by its Change number and Patch Set number.

### Fetch and Checkout a Specific Change

```bash
# Format: refs/changes/LAST_2_DIGITS/CHANGE_NUMBER/PATCH_SET
# For Change 12345, Patch Set 3:
git fetch origin refs/changes/45/12345/3
git checkout FETCH_HEAD
```

### Understanding the ref path

```
refs/changes/45/12345/3
             ││  │     │
             ││  │     └── Patch Set number
             ││  └──────── Change number
             │└──────────── Last 2 digits of Change number
             └───────────── Always "refs/changes/"
```

### Checkout Into a Named Branch

```bash
git fetch origin refs/changes/45/12345/3
git checkout -b review-12345 FETCH_HEAD
```

### Fetch the Latest Patch Set

```bash
# If you don't know the latest patch set number, use the Gerrit query:
git ls-remote origin | grep 12345

# Or fetch all patch sets
git fetch origin refs/changes/45/12345/*
```

### Cherry-Pick a Change Onto Your Branch

```bash
git fetch origin refs/changes/45/12345/3
git cherry-pick FETCH_HEAD
```

### Pull a Change Using `git-review` Tool (if installed)

```bash
# Much simpler syntax:
git review -d 12345
# Creates a branch and checks out the latest patch set
```

---

## 🔹 8. Reviewing & Voting

### The Concept

In Gerrit, reviewing happens through **votes** on **labels**. The two most common labels are:

| Label | Range | Meaning |
|-------|:-----:|---------|
| **Code-Review** | -2 to +2 | `-2` = block, `-1` = needs work, `0` = neutral, `+1` = looks good, `+2` = approved |
| **Verified** | -1 to +1 | `-1` = build failed, `+1` = build passed (usually from CI) |

### Voting from the Command Line (SSH)

```bash
# Give Code-Review +2
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 --code-review +2

# Give Code-Review +2 with a message
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 \
    --code-review +2 \
    --message '"Looks great, approved!"'

# Give Verified +1
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 --verified +1

# Vote on multiple labels at once
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 \
    --code-review +2 --verified +1
```

> **Note:** `12345,3` means Change number 12345, Patch Set 3.

### Voting via REST API

```bash
# Code-Review +2
curl -X POST \
    -H "Content-Type: application/json" \
    -d '{"labels": {"Code-Review": 2}, "message": "LGTM!"}' \
    "https://gerrit.example.com/a/changes/12345/revisions/current/review" \
    --user username:http-password
```

### Adding Inline Comments (SSH)

```bash
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 \
    --message '"Overall looks good, but see inline comments."' \
    --code-review +1
```

> 💡 **Tip:** Most inline commenting is done through the Gerrit web UI. Use SSH/REST for automated workflows (CI, bots, scripts).

---

## 🔹 9. Submitting & Merging

### The Concept

**Submitting** is Gerrit's equivalent of merging a Pull Request. A Change can be submitted when:
- It has the required approvals (typically `Code-Review +2` and `Verified +1`)
- There are no blocking votes (`Code-Review -2`)
- It passes any configured submit rules

### Submit via SSH

```bash
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 --submit
```

### Submit via REST API

```bash
curl -X POST \
    "https://gerrit.example.com/a/changes/12345/submit" \
    --user username:http-password
```

### Submit Strategies

The project administrator configures how Changes are merged:

| Strategy | What Happens |
|----------|-------------|
| **Merge if Necessary** | Fast-forward if possible, otherwise merge commit |
| **Always Merge** | Always creates a merge commit |
| **Cherry Pick** | Cherry-picks the Change onto the target branch |
| **Rebase if Necessary** | Rebases if not up-to-date, then fast-forward |
| **Rebase Always** | Always rebases before merging |

### Abandon a Change

```bash
# Via SSH
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 --abandon

# With a reason
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 \
    --abandon --message '"Superseded by Change 12400"'
```

### Restore an Abandoned Change

```bash
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 --restore
```

---

## 🔹 10. Topic & Relation Chains

### The Concept

Sometimes a feature requires **multiple Changes** across different repos or in a specific order. Gerrit provides two mechanisms for this:

### Topics — Grouping Related Changes

A **topic** is a tag that groups related Changes across one or more repositories.

```bash
# Push with a topic
git push origin HEAD:refs/for/master%topic=feature-dark-mode

# Or set via push option
git push origin HEAD:refs/for/master -o topic=feature-dark-mode
```

All Changes with the same topic appear together in the Gerrit UI. If **Submit Whole Topic** is enabled, submitting one submits all.

### Relation Chains — Stacked Changes

If you push multiple commits at once, Gerrit creates a **relation chain** (stacked Changes):

```bash
# You have 3 commits on top of master
git log --oneline -3
# abc1234 Add unit tests        ← Change #12347
# def5678 Implement feature     ← Change #12346
# 789abcd Add interface         ← Change #12345

# Push all three
git push origin HEAD:refs/for/master

# Gerrit creates 3 Changes, linked in order:
#   #12345 → #12346 → #12347
```

In the Gerrit UI, you'll see a "Relation chain" panel showing the dependency order.

> ⚠️ **Warning:** If you submit Change #12346 before #12345, the target branch won't have the interface code yet. Gerrit will try to rebase or warn you.

---

## 🔹 11. Cherry-Pick & Rebase

### Cherry-Pick a Change to Another Branch

Sometimes a fix needs to go to both `master` and a `release` branch:

```bash
# Method 1: From Gerrit web UI
# Open the Change → Actions → Cherry Pick → select target branch

# Method 2: Locally
git fetch origin refs/changes/45/12345/3
git checkout release/2.0
git cherry-pick FETCH_HEAD
# The hook adds a NEW Change-Id → creates a new Change on release/2.0
git push origin HEAD:refs/for/release/2.0
```

### Cherry-Pick via SSH

```bash
ssh -p 29418 username@gerrit.example.com gerrit review 12345,3 \
    --cherry-pick release/2.0
```

### Rebase a Change

When the target branch has moved ahead and your Change has a merge conflict or is outdated:

```bash
# Method 1: Rebase locally
git fetch origin master
git rebase origin/master
# Resolve any conflicts
git push origin HEAD:refs/for/master

# Method 2: Rebase via REST API (no conflicts only)
curl -X POST \
    "https://gerrit.example.com/a/changes/12345/rebase" \
    --user username:http-password
```

### Rebase via Gerrit Web UI

Open the Change → click **Rebase** button. If there are conflicts, you'll need to rebase locally.

---

## 🔹 12. Gerrit SSH Commands

### The Concept

Gerrit exposes an SSH interface for administrative and review operations. These commands are the backbone of CI/CD integration and automation scripts.

### Query Changes

```bash
# Find open changes by a user
ssh -p 29418 username@gerrit.example.com gerrit query \
    --format=JSON status:open owner:alice

# Find changes in a project
ssh -p 29418 username@gerrit.example.com gerrit query \
    --format=JSON project:my-project status:open

# Find changes needing your review
ssh -p 29418 username@gerrit.example.com gerrit query \
    --format=JSON reviewer:self status:open

# Search by topic
ssh -p 29418 username@gerrit.example.com gerrit query \
    --format=JSON topic:feature-dark-mode

# Limit results  
ssh -p 29418 username@gerrit.example.com gerrit query \
    --format=JSON --current-patch-set status:open limit:10
```

### Common Query Operators

| Operator | Example | Meaning |
|----------|---------|---------|
| `status:` | `status:open` | Open, merged, abandoned |
| `owner:` | `owner:self` | Changes you created |
| `reviewer:` | `reviewer:alice` | Changes where alice is a reviewer |
| `project:` | `project:my-repo` | Changes in the specified project |
| `branch:` | `branch:master` | Changes targeting a branch |
| `topic:` | `topic:bugfix` | Changes with a specific topic |
| `label:` | `label:Code-Review+2` | Changes with a specific vote |
| `is:` | `is:wip` | WIP changes |
| `age:` | `age:7d` | Changes older than 7 days |
| `message:` | `message:fix` | Commit message contains "fix" |
| `file:` | `file:*.cpp` | Changes touching .cpp files |
| `has:` | `has:unresolved` | Has unresolved comments |

### Combine Operators

```bash
# Open changes by alice on master with Code-Review +1 but not +2
ssh -p 29418 username@gerrit.example.com gerrit query \
    "status:open owner:alice branch:master label:Code-Review=1 -label:Code-Review=2"
```

### Other SSH Commands

```bash
# List projects
ssh -p 29418 username@gerrit.example.com gerrit ls-projects

# List groups
ssh -p 29418 username@gerrit.example.com gerrit ls-groups

# List members of a group
ssh -p 29418 username@gerrit.example.com gerrit ls-members "Project Owners"

# Stream events (real-time feed — great for CI triggers)
ssh -p 29418 username@gerrit.example.com gerrit stream-events

# Create a new project
ssh -p 29418 username@gerrit.example.com gerrit create-project new-project \
    --parent All-Projects

# Set a reviewer
ssh -p 29418 username@gerrit.example.com gerrit set-reviewers \
    -a alice@company.com -a bob@company.com 12345
```

---

## 🔹 13. Gerrit REST API (curl)

### The Concept

Gerrit exposes a full REST API for everything you can do in the web UI. This is ideal for scripting, CI/CD pipelines, and custom dashboards.

> **Authentication:** Use HTTP credentials (Settings → HTTP Credentials in Gerrit UI).

### Query Changes

```bash
# Get open changes
curl -s "https://gerrit.example.com/a/changes/?q=status:open+project:my-project" \
    --user username:http-password | tail -c +6 | jq .

# Note: Gerrit prefixes JSON responses with )]}' for XSS protection.
# tail -c +6 strips that prefix.
```

### Get Change Details

```bash
# Full details with current revision
curl -s "https://gerrit.example.com/a/changes/12345/detail" \
    --user username:http-password | tail -c +6 | jq .
```

### Get Diff

```bash
# Get the diff for a specific file
curl -s "https://gerrit.example.com/a/changes/12345/revisions/current/files/src%2Fmain.cpp/diff" \
    --user username:http-password | tail -c +6 | jq .
```

### Post a Review

```bash
curl -X POST \
    -H "Content-Type: application/json" \
    -d '{
        "message": "Automated review: build passed",
        "labels": {
            "Verified": 1
        }
    }' \
    "https://gerrit.example.com/a/changes/12345/revisions/current/review" \
    --user username:http-password
```

### Add Reviewer

```bash
curl -X POST \
    -H "Content-Type: application/json" \
    -d '{"reviewer": "alice@company.com"}' \
    "https://gerrit.example.com/a/changes/12345/reviewers" \
    --user username:http-password
```

### Common REST Endpoints

| Method | Endpoint | Purpose |
|:------:|----------|---------|
| GET | `/changes/?q=...` | Query changes |
| GET | `/changes/{id}/detail` | Full change details |
| GET | `/changes/{id}/revisions/current/files` | List modified files |
| POST | `/changes/{id}/revisions/current/review` | Vote / comment |
| POST | `/changes/{id}/submit` | Submit (merge) |
| POST | `/changes/{id}/abandon` | Abandon change |
| POST | `/changes/{id}/restore` | Restore abandoned change |
| POST | `/changes/{id}/rebase` | Rebase change |
| POST | `/changes/{id}/reviewers` | Add reviewer |
| GET | `/projects/` | List projects |
| GET | `/accounts/self` | Get current user info |

---

## 🔹 14. Branch & Tag Management

### Create a Branch

```bash
# Via SSH
ssh -p 29418 username@gerrit.example.com gerrit create-branch \
    my-project release/3.0 master

# Via REST
curl -X PUT \
    -H "Content-Type: application/json" \
    -d '{"revision": "master"}' \
    "https://gerrit.example.com/a/projects/my-project/branches/release%2F3.0" \
    --user username:http-password
```

### Delete a Branch

```bash
# Via REST
curl -X DELETE \
    "https://gerrit.example.com/a/projects/my-project/branches/feature%2Fold" \
    --user username:http-password

# Via Git (if permitted)
git push origin --delete feature/old
```

### Create a Tag

```bash
# Lightweight tag via REST
curl -X PUT \
    -H "Content-Type: application/json" \
    -d '{"revision": "master"}' \
    "https://gerrit.example.com/a/projects/my-project/tags/v3.0.0" \
    --user username:http-password

# Annotated tag via Git
git tag -a v3.0.0 -m "Release 3.0.0"
git push origin v3.0.0
```

---

## 🔹 15. Troubleshooting & Tips

### Common Errors and Fixes

---

**Error:** `! [remote rejected] HEAD -> refs/for/master (missing Change-Id in message footer)`

```bash
# Fix: Install the hook and amend
scp -p -P 29418 username@gerrit.example.com:hooks/commit-msg .git/hooks/
git commit --amend --no-edit
git push origin HEAD:refs/for/master
```

---

**Error:** `! [remote rejected] HEAD -> refs/for/master (no new changes)`

This means the commit you're pushing already exists as a Patch Set on Gerrit.

```bash
# Fix: Make a change and amend
git commit --amend   # Edit the message or code
git push origin HEAD:refs/for/master
```

---

**Error:** `! [remote rejected] HEAD -> refs/for/master (change closed)`

The Change was already merged or abandoned.

```bash
# Fix: Remove the old Change-Id and push as a new Change
git commit --amend
# Delete the Change-Id line from the message → hook generates a new one
git push origin HEAD:refs/for/master
```

---

**Error:** `Permission denied (publickey)`

```bash
# Fix: Verify your SSH key is added to Gerrit
ssh -p 29418 username@gerrit.example.com gerrit version
# If that fails, add your key: Settings → SSH Keys in Gerrit web UI
```

---

### Useful Tips

| Tip | Command |
|-----|---------|
| See what will be pushed | `git log origin/master..HEAD --oneline` |
| Push only specific commits | `git push origin <commit-sha>:refs/for/master` |
| Reset to clean state | `git fetch origin && git reset --hard origin/master` |
| Squash multiple commits into one | `git rebase -i HEAD~N` then squash |
| Compare local vs remote | `git diff origin/master..HEAD` |
| Check Gerrit connectivity | `ssh -p 29418 user@gerrit.example.com gerrit version` |

### The `git-review` Shortcut Tool

If you work with Gerrit frequently, install `git-review`:

```bash
pip install git-review

# Configure (first time in a repo)
git review -s

# Push for review (replaces the long git push command)
git review

# Download a change for review
git review -d 12345

# Compare patch sets
git review -d 12345 --diff 1..3
```

---

## 🔹 16. Quick Reference Cheat Sheet

### Daily Workflow

```bash
# Start work
git checkout -b my-feature origin/master

# Commit
git add -A && git commit -m "Implement feature X"

# Push for review
git push origin HEAD:refs/for/master

# Update after feedback
git add -A && git commit --amend --no-edit
git push origin HEAD:refs/for/master

# Checkout someone's change
git fetch origin refs/changes/45/12345/3 && git checkout FETCH_HEAD
```

### Push Options Quick Reference

```bash
git push origin HEAD:refs/for/<branch>                           # Basic push
git push origin HEAD:refs/for/<branch>%r=reviewer@email.com      # Add reviewer
git push origin HEAD:refs/for/<branch>%topic=my-topic            # Set topic
git push origin HEAD:refs/for/<branch>%wip                       # Work in progress
git push origin HEAD:refs/for/<branch>%ready                     # Mark ready
git push origin HEAD:refs/for/<branch>%hashtag=sprint-42         # Add hashtag
git push origin HEAD:refs/for/<branch>%l=Code-Review+2           # Set label
git push origin HEAD:refs/for/<branch>%cc=someone@email.com      # Add CC
```

### SSH Commands Quick Reference

```bash
ssh -p 29418 user@host gerrit version                            # Test connection
ssh -p 29418 user@host gerrit query status:open owner:self       # My open changes
ssh -p 29418 user@host gerrit review 12345,1 --code-review +2   # Approve
ssh -p 29418 user@host gerrit review 12345,1 --submit           # Merge
ssh -p 29418 user@host gerrit review 12345,1 --abandon          # Abandon
ssh -p 29418 user@host gerrit set-reviewers -a email 12345      # Add reviewer
ssh -p 29418 user@host gerrit ls-projects                        # List projects
ssh -p 29418 user@host gerrit stream-events                      # Real-time events
```

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <br/>
  <em>🎓 Gerrit Commands — Complete Reference</em><br/>
  <em>From first clone to production submit.</em><br/>
  <br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
