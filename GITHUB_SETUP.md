# How to Push to GitHub

Follow these steps to push your MakeItHappenOTT plugin to GitHub.

## Step 1: Create a GitHub Repository

1. Go to https://github.com
2. Click the **"+"** icon in the top right → **"New repository"**
3. Fill in the details:
   - **Repository name**: `MakeItHappenOTT` (or your preferred name)
   - **Description**: "Professional multiband OTT compressor plugin built with JUCE"
   - **Visibility**: Choose Public or Private
   - **DO NOT** check "Initialize with README" (we already have one)
4. Click **"Create repository"**

## Step 2: Initialize Git in Your Project

Open Terminal and navigate to your project:

```bash
cd /Users/davidyousefi/dev/MakeItHappenOTT
```

Initialize git (if not already done):

```bash
git init
```

## Step 3: Add All Files

Add all your files to git:

```bash
git add .
```

Check what will be committed:

```bash
git status
```

## Step 4: Create Your First Commit

```bash
git commit -m "Initial commit: MakeItHappenOTT v0.0.1

- 3-band multiband OTT compression
- Stereo width controls per band
- Solo buttons for each band
- Gain match toggle
- Custom vector UI elements
- VST3 and AU support"
```

## Step 5: Connect to GitHub

Replace `YOUR_USERNAME` with your actual GitHub username:

```bash
git remote add origin https://github.com/YOUR_USERNAME/MakeItHappenOTT.git
```

Verify the remote was added:

```bash
git remote -v
```

## Step 6: Push to GitHub

For the first push, use:

```bash
git branch -M main
git push -u origin main
```

You'll be prompted for your GitHub credentials. If you have 2FA enabled, you'll need to use a **Personal Access Token** instead of your password.

### Creating a Personal Access Token (if needed):

1. Go to GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Click "Generate new token (classic)"
3. Give it a name like "MakeItHappenOTT"
4. Select scopes: Check **"repo"** (full control of private repositories)
5. Click "Generate token"
6. **COPY THE TOKEN** - you won't see it again!
7. Use this token as your password when pushing

## Step 7: Verify Upload

Go to your repository on GitHub:
```
https://github.com/YOUR_USERNAME/MakeItHappenOTT
```

You should see all your files!

## What Gets Pushed

The `.gitignore` file ensures only source code is pushed, not:
- Build folders (`build/`)
- IDE files (`.vscode/`, `.idea/`)
- Compiled binaries (`.vst3`, `.component`)
- macOS system files (`.DS_Store`)

## Automatic Windows Builds

Once pushed, GitHub Actions will automatically build Windows and macOS versions!

1. Go to your repository → **Actions** tab
2. You'll see builds running automatically
3. When complete, click on a build to download the artifacts
4. Download the **Windows VST3** artifact to send to Windows users

## Future Updates

After making changes to your code:

```bash
git add .
git commit -m "Description of your changes"
git push
```

## Useful Git Commands

Check current status:
```bash
git status
```

View commit history:
```bash
git log --oneline
```

See what changed:
```bash
git diff
```

Undo uncommitted changes:
```bash
git checkout -- filename
```

## Troubleshooting

### "Repository not found"
- Make sure you replaced `YOUR_USERNAME` with your actual GitHub username
- Check that the repository exists on GitHub

### "Authentication failed"
- If using 2FA, use a Personal Access Token instead of your password
- Make sure you have permission to push to the repository

### "Permission denied (publickey)"
- You need to set up SSH keys or use HTTPS with a token
- Easier to use HTTPS for now

### Large files rejected
- Build artifacts are ignored by `.gitignore`
- If you get errors, make sure you didn't modify `.gitignore`

## Need Help?

- GitHub Docs: https://docs.github.com/en/get-started
- Git Cheat Sheet: https://education.github.com/git-cheat-sheet-education.pdf
