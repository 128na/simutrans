# Simutrans 開発環境セットアップガイド

## 概要

このガイドでは、Simutrans の開発環境をセットアップする手順を説明します。

## 前提条件

### 共通要件

- Git または SVN クライアント
- 5GB 以上のディスク空き容量
- インターネット接続（依存ライブラリのダウンロード用）

## プラットフォーム別セットアップ

### Windows

#### オプション 1: Visual Studio（推奨：デバッグ向け）

##### 1. Visual Studio のインストール

- [Visual Studio 2019](https://visualstudio.microsoft.com/) 以降をインストール
- 必要なワークロード：
  - C++によるデスクトップ開発
  - CMake ツール（オプション）

##### 2. vcpkg のインストール

```powershell
# vcpkgをクローン
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# vcpkgをビルド
.\bootstrap-vcpkg.bat

# 環境変数を設定（オプション）
setx VCPKG_ROOT "%CD%"
```

##### 3. 依存ライブラリのインストール

```powershell
# Simutransのリポジトリに移動
cd c:\dev\simutrans

# アーキテクチャに応じたスクリプトをvcpkgフォルダにコピーして実行
# x64の場合:
copy tools\install-building-libs-x64.bat [vcpkg-root]\
cd [vcpkg-root]
.\install-building-libs-x64.bat
```

または手動でインストール：

```powershell
.\vcpkg install freetype:x64-windows-static
.\vcpkg install miniupnpc:x64-windows-static
.\vcpkg install pthread:x64-windows-static
.\vcpkg install zstd:x64-windows-static
.\vcpkg install sdl2:x64-windows-static
.\vcpkg install zlib:x64-windows-static
```

##### 4. プロジェクトを開く

1. `Simutrans.sln`を Visual Studio で開く
2. スタートアッププロジェクトを選択：
   - **Simutrans SDL2**: 推奨
   - **Simutrans GDI**: Windows 専用
   - **Simutrans Server**: GUI 無し
3. ビルド構成を選択（Debug/Release）
4. F5 キーでビルド＆実行

#### オプション 2: MSYS2（推奨：ビルドの簡易性）

##### 1. MSYS2 のインストール

1. [MSYS2](https://www.msys2.org/)をダウンロードしてインストール
2. MSYS2 MinGW 64-bit ターミナルを起動

##### 2. セットアップスクリプトの実行

```bash
cd /c/dev/simutrans
./tools/setup-mingw.sh
```

このスクリプトは自動的に：

- 必要なパッケージをインストール
- ビルド環境を設定

##### 3. ビルド

```bash
# configure（初回のみ）
./configure

# ビルド
make -j$(nproc)
```

### Linux

#### Ubuntu/Debian 系

##### 1. セットアップスクリプトの実行

```bash
cd ~/dev/simutrans
sudo ./tools/setup-debian.sh
```

このスクリプトは以下をインストール：

- ビルドツール（gcc, g++, make, autoconf）
- 必須ライブラリ（zlib, libpng, freetype, SDL2）
- オプションライブラリ（zstd, miniupnpc, fluidsynth）

##### 2. 手動インストール（代替）

```bash
# ビルドツール
sudo apt-get install build-essential autoconf pkg-config

# 必須ライブラリ
sudo apt-get install zlib1g-dev
sudo apt-get install libpng-dev
sudo apt-get install libfreetype6-dev
sudo apt-get install libbz2-dev
sudo apt-get install libsdl2-dev

# オプションライブラリ
sudo apt-get install libzstd-dev
sudo apt-get install libminiupnpc-dev
sudo apt-get install libfluidsynth-dev
sudo apt-get install libfontconfig1-dev
```

##### 3. ビルド

```bash
# configure
autoconf
./configure

# ビルド（コア数に応じて-jを調整）
make -j$(nproc)

# 実行ファイルは build/default/sim に生成される
```

#### Arch Linux

```bash
# ビルドツール
sudo pacman -S base-devel autoconf pkg-config

# 必須ライブラリ
sudo pacman -S zlib libpng freetype2 bzip2 sdl2

# オプションライブラリ
sudo pacman -S zstd miniupnpc fluidsynth fontconfig
```

#### Fedora/RHEL

```bash
# ビルドツール
sudo dnf install gcc gcc-c++ make autoconf pkgconfig

# 必須ライブラリ
sudo dnf install zlib-devel libpng-devel freetype-devel bzip2-devel SDL2-devel

# オプションライブラリ
sudo dnf install libzstd-devel miniupnpc-devel fluidsynth-devel fontconfig-devel
```

### macOS

#### 1. Homebrew のインストール

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### 2. 依存ライブラリのインストール

```bash
# ビルドツール
xcode-select --install

# 依存ライブラリ
brew install autoconf automake
brew install sdl2
brew install freetype
brew install libpng
brew install zstd
brew install miniupnpc
brew install pkg-config
```

#### 3. ビルド

```bash
cd ~/dev/simutrans

# configure
autoreconf -ivf
./configure

# ビルド
make -j$(sysctl -n hw.ncpu)

# バージョン情報生成
make OSX/getversion
```

## CMake を使用したビルド

### Linux/macOS/MinGW

```bash
# ビルドディレクトリを作成
mkdir build
cd build

# 設定（ジェネレーターを指定）
cmake -G "Unix Makefiles" ..

# または Ninjaを使用（高速）
# cmake -G "Ninja" ..

# ビルド
cmake --build . -j$(nproc)  # Linux
cmake --build . -j$(sysctl -n hw.ncpu)  # macOS

# 実行ファイルは build/simutrans/ に生成される
```

### Windows (Visual Studio)

```powershell
# ビルドディレクトリを作成
mkdir build
cd build

# 設定
cmake.exe .. -G "Visual Studio 16 2019" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake

# ビルド
cmake --build . --config Release

# または Visual Studioで開く
# simutrans.slnが生成される
```

### CMake オプション

```bash
# バックエンドの選択
cmake .. -DSIMUTRANS_BACKEND=sdl2    # SDL2（デフォルト）
cmake .. -DSIMUTRANS_BACKEND=gdi     # GDI（Windows）
cmake .. -DSIMUTRANS_BACKEND=none    # サーバー

# ビルドタイプ
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# リビジョン番号を手動設定
cmake .. -DSIMUTRANS_USE_REVISION=12345
```

## ソースコードの取得

### SVN（推奨：公式リポジトリ）

```bash
svn checkout svn://servers.simutrans.org/simutrans/trunk simutrans
```

### Git（ミラー）

```bash
git clone https://github.com/simutrans/simutrans.git
```

**注意**: Git ミラーを使用する場合、ネットワークゲーム参加時は手動でバージョンを設定する必要があります。

## ゲームデータの取得

ビルドしても実行ファイルのみが生成されます。ゲームを実行するには、Pakset が必要です。

### 方法 1: スクリプトを使用（推奨）

#### Windows (PowerShell)

```powershell
cd simutrans
.\tools\get_pak.ps1
```

#### Linux/macOS

```bash
cd simutrans
./tools/get_pak.sh
```

### 方法 2: 手動ダウンロード

1. [Paksets](https://www.simutrans.com/en/paksets/)から好みの Pakset をダウンロード
2. simutrans/ディレクトリに展開

推奨 Pakset：

- **pak64**: 標準、軽量
- **pak128**: 高解像度、詳細
- **pak128.britain**: イギリステーマ

## 実行

### 既存の Simutrans インストールを使用

```bash
# -use_workdirで既存のインストールを指定
./build/default/sim -use_workdir /path/to/simutrans
```

### スタンドアロン実行

Pakset をダウンロード後：

```bash
cd simutrans
../build/default/sim
```

## IDE の設定

### Visual Studio Code

#### 推奨拡張機能

- C/C++ (Microsoft)
- CMake Tools
- clangd (オプション、より良いインテリセンス)

#### settings.json

```json
{
	"C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
	"cmake.buildDirectory": "${workspaceFolder}/build",
	"cmake.configureSettings": {
		"CMAKE_BUILD_TYPE": "Debug",
		"SIMUTRANS_BACKEND": "sdl2"
	}
}
```

#### tasks.json (Make 用)

```json
{
	"version": "2.0.0",
	"tasks": [
		{
			"label": "Build Simutrans",
			"type": "shell",
			"command": "make",
			"args": ["-j8"],
			"group": {
				"kind": "build",
				"isDefault": true
			},
			"problemMatcher": ["$gcc"]
		}
	]
}
```

### Visual Studio

- `Simutrans.sln`を開く
- プロジェクトプロパティは自動設定済み
- vcpkg が正しく設定されていることを確認

### CLion

1. プロジェクトを開く（CMakeLists.txt を選択）
2. CMake 設定を調整：
   - Settings > Build, Execution, Deployment > CMake
   - CMake options に追加：
     ```
     -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
     ```

## トラブルシューティング

### 依存ライブラリが見つからない

**Linux/macOS:**

```bash
# pkg-configのパスを確認
pkg-config --list-all | grep -i sdl2

# パスを追加（必要に応じて）
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```

**Windows (MSVC):**

- vcpkg が正しくインストールされているか確認
- CMAKE_TOOLCHAIN_FILE が正しく設定されているか確認

### ビルドエラー: 'SDL.h' file not found

**解決方法:**

```bash
# SDL2が正しくインストールされているか確認
# Linux/macOS
pkg-config --cflags sdl2

# ない場合は再インストール
sudo apt-get install --reinstall libsdl2-dev  # Ubuntu/Debian
brew reinstall sdl2  # macOS
```

### リンクエラー

**Linux:**

```bash
# 不足しているライブラリを確認
ldd build/default/sim

# 必要に応じてライブラリを追加
sudo ldconfig
```

### macOS での権限エラー

```bash
# Homebrewのパーミッション修正
sudo chown -R $(whoami) /usr/local/lib /usr/local/include
```

### Windows: 'configure'が見つからない

- MSYS ターミナルを使用していることを確認
- パス区切りは`/`を使用（`\`ではなく）

## 高度な設定

### クロスコンパイル（Linux → Windows）

```bash
# MinGWツールチェーンをインストール
sudo apt-get install mingw-w64

# configure時にホストを指定
./configure --host=x86_64-w64-mingw32

# ビルド
make -j$(nproc)
```

詳細: [Cross-Compiling Simutrans](https://simutrans-germany.com/wiki/wiki/en_Cross-Compiling_Simutrans)

### マルチスレッドビルド

ビルド時間を短縮：

```bash
# CPU コア数を確認
# Linux:
nproc
# macOS:
sysctl -n hw.ncpu

# その数でビルド
make -j8  # 8コアの場合
```

### デバッグビルド

```bash
# Makeの場合
make FLAGS="-g -O0"

# CMakeの場合
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### リリースビルドの最適化

```bash
# Makeの場合
make FLAGS="-O3 -march=native"

# CMakeの場合
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## 次のステップ

1. [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)でプロジェクト構造を理解
2. [TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md)でアーキテクチャを学習
3. `documentation/coding_styles.txt`でコーディング規約を確認
4. [Forum](https://forum.simutrans.com/)でコミュニティに参加

## 参考リンク

- [Compiling Simutrans Wiki](https://simutrans-germany.com/wiki/wiki/en_CompilingSimutrans)
- [Development Index](https://simutrans-germany.com/wiki/wiki/en_Devel_Index)
- [Technical Documentation Forum](https://forum.simutrans.com/index.php/board,112.0.html)
