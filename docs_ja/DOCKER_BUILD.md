# Docker 環境での Simutrans ビルド

## 概要

Docker を使用することで、ホスト環境を汚さずに一貫したビルド環境を構築できます。すべての依存関係が Docker イメージ内に含まれるため、セットアップが簡単です。

## 必要なもの

- **Docker Desktop** (Windows/Mac) または **Docker Engine** (Linux)
- **Docker Compose** (通常 Docker に含まれる)
- 8GB 以上のディスク空き容量

## Docker のインストール

### Windows / macOS

1. [Docker Desktop](https://www.docker.com/products/docker-desktop/)をダウンロードしてインストール
2. Docker Desktop を起動
3. 動作確認：
   ```powershell
   docker --version
   docker-compose --version
   ```

### Linux

```bash
# Ubuntu/Debian
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker $USER

# Docker Compose
sudo apt-get install docker-compose-plugin

# 再ログイン後、確認
docker --version
docker compose version
```

## クイックスタート

### 方法 1: Docker Compose（推奨）

```bash
# プロジェクトディレクトリに移動
cd c:/dev/simutrans

# Dockerイメージをビルド
docker-compose build

# コンテナを起動してビルド
docker-compose run --rm simutrans-build docker-build.sh make

# または、コンテナに入ってインタラクティブに作業
docker-compose run --rm simutrans-build
# コンテナ内で:
# ./docker-build.sh make
```

### 方法 2: Docker コマンド直接

```bash
# Dockerイメージをビルド
docker build -t simutrans-build .

# コンテナ内でビルド
docker run --rm -v ${PWD}:/simutrans simutrans-build docker-build.sh make

# または、コンテナに入る
docker run -it --rm -v ${PWD}:/simutrans simutrans-build bash
```

## ビルド方法

### Make を使用

```bash
# Docker Composeの場合
docker-compose run --rm simutrans-build docker-build.sh make

# または手動で
docker-compose run --rm simutrans-build bash
# コンテナ内:
autoconf
./configure
make -j$(nproc)
```

### CMake を使用

```bash
# Docker Composeの場合
docker-compose run --rm simutrans-build docker-build.sh cmake

# または手動で
docker-compose run --rm simutrans-build bash
# コンテナ内:
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DSIMUTRANS_BACKEND=sdl2 ..
cmake --build . -j$(nproc)
```

## Windows でのビルド

### 方法 A: Docker で MinGW クロスコンパイル（推奨）

Docker 環境（Linux）から Windows 用バイナリ（.exe）を生成できます。

#### 前提条件

- Docker Desktop (Windows/Mac) または Docker Engine (Linux)
- Docker Compose

#### ビルド手順

```bash
# MinGWイメージをビルド（初回のみ）
docker-compose build simutrans-mingw

# Windows用バイナリをビルド
docker-compose run --rm simutrans-mingw docker-build-mingw.sh make

# または、コンテナに入って手動でビルド
docker-compose run --rm simutrans-mingw bash
# コンテナ内:
./docker-build-mingw.sh make

# ビルド成果物を確認
ls -lh sim.exe
```

#### 生成されるファイル

- **sim.exe** - Windows 用実行ファイル（64-bit）
- プロジェクトルートに直接生成されます

#### MinGW クロスコンパイルの利点

✅ **クロスプラットフォーム**: Linux や macOS から Windows バイナリをビルド可能
✅ **環境の一貫性**: Docker コンテナで依存関係が完全に管理される
✅ **CI/CD 対応**: GitHub Actions などで自動ビルド可能
✅ **Windows 不要**: Windows マシンが不要でビルド可能

#### 注意事項

⚠️ MinGW でビルドされたバイナリは、Windows 用の依存 DLL が必要な場合があります。配布時は依存関係を確認してください。

#### DLL のステージング（配布/実行準備）

ビルド済みの `sim.exe` と必要な DLL をまとめて `dist/win64/` にコピーします。

```bash
# 必要なDLLを収集してステージング
docker-compose run --rm simutrans-mingw ./docker-build-mingw.sh stage

# Windows側での起動例（PowerShell）
.\u005cdist\win64\sim.exe
```

`dist/win64/` に含まれる主なファイル:

- sim.exe
- SDL2.dll
- libfreetype-6.dll
- libpng16-16.dll
- zlib1.dll
- libstdc++-6.dll
- libgcc_s_seh-1.dll
- libwinpthread-1.dll

（Windows 標準の DLL はコピーされません。上記を同梱すれば DLL 不足エラーが解消されます。）

### 方法 B: Windows ネイティブビルド

#### 前提条件

- **vcpkg** (Microsoft の C++ パッケージ マネージャー)
- **PowerShell** (ビルドスクリプト実行用)

詳細は [DEVELOPMENT_SETUP.md](DEVELOPMENT_SETUP.md) の Windows セクションを参照してください。

### ビルド方法

#### 方法 1: Visual Studio から直接ビルド

1. Simutrans.sln を Visual Studio で開く
2. プロジェクトを選択：
   - **Simutrans-GDI** : Windows GDI (32-bit)
   - **Simutrans-SDL2** : SDL2 (64-bit)
3. Release 構成を選択
4. Build → Build Solution を実行

#### 方法 2: PowerShell スクリプト (推奨)

```powershell
# PowerShell を「管理者として実行」で開く

# vcpkg のセットアップ
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
cd ..

# MSBuild でビルド（32-bit GDI）
msbuild /m .\Simutrans-GDI.vcxproj -p:Configuration=Release /p:platform=Win32

# または、64-bit SDL2
msbuild /m .\Simutrans-SDL2.vcxproj -p:Configuration=Release /p:platform=x64

# ビルドファイルの確認
# build/GDI/Simutrans GDI Nightly.exe    (32-bit GDI版)
# build/SDL2/Simutrans SDL2 Nightly.exe  (64-bit SDL2版)
```

#### 方法 3: GitHub Actions ワークフロー（CI/CD）

プロジェクトには自動ビルドワークフローが含まれています：

- `.github/workflows/nightly-windows-ms.yml` - 32-bit GDI 版
- `.github/workflows/nightly-windows64-SDL2-ms.yml` - 64-bit SDL2 版

これらはプッシュ時に自動実行され、リリースファイルが生成されます。

### Windows ビルドの詳細

#### バックエンド選択

| バックエンド | プロジェクト           | ビット数 | 用途                                |
| ------------ | ---------------------- | -------- | ----------------------------------- |
| **GDI**      | Simutrans-GDI.vcxproj  | 32-bit   | 古い Windows との互換性が必要な場合 |
| **SDL2**     | Simutrans-SDL2.vcxproj | 64-bit   | 推奨（より新しく、機能が豊富）      |

#### vcpkg 依存関係

Windows ビルドでは、vcpkg を通じて以下のライブラリが自動インストールされます：

```
- zlib
- libpng
- freetype
- bzip2
- zstd
- miniupnpc
- fluidsynth
- fontconfig
```

詳細は `vcpkg.json` を参照してください。

### トラブルシューティング (Windows)

**vcpkg インストールエラー**

```powershell
# vcpkg キャッシュをクリア
.\vcpkg remove '*' --outdated
.\vcpkg update
```

**MSBuild が見つからない**

```powershell
# Visual Studio のインストール確認
Get-Command msbuild

# または、Visual Studio Installer から C++ ワークロードを追加
```

**ビルドファイルが見つからない**

```powershell
# 64-bit GDI 版（推奨）
# build/GDI/Simutrans GDI Nightly.exe

# または 32-bit SDL2 版
# build/SDL2/Simutrans SDL2 Nightly.exe
```

## docker-build.sh の使い方

便利なビルドスクリプトが用意されています：

```bash
# Makeでビルド（デフォルト）
docker-compose run --rm simutrans-build docker-build.sh make

# CMakeでビルド
docker-compose run --rm simutrans-build docker-build.sh cmake

# クリーン
docker-compose run --rm simutrans-build docker-build.sh clean
```

## VSCode Dev Container

VSCode の Dev Container 機能を使用すると、統合開発環境として使用できます。

### 使い方

1. VSCode を開く
2. 拡張機能「Dev Containers」をインストール
3. プロジェクトフォルダを開く
4. コマンドパレット（Ctrl+Shift+P / Cmd+Shift+P）を開く
5. 「Dev Containers: Reopen in Container」を選択

これで、VSCode が自動的に Docker コンテナ内で動作するようになります。

### 利点

- IntelliSense が正しく動作
- デバッガーが使用可能
- ターミナルがコンテナ内で動作
- 拡張機能がコンテナ内で動作

## ファイル構成

```
simutrans/
├── Dockerfile                 # Dockerイメージ定義
├── docker-compose.yml         # Docker Compose設定
├── docker-build.sh           # ビルドスクリプト
├── .dockerignore             # Docker除外ファイル
└── .devcontainer/            # VSCode Dev Container設定
    └── devcontainer.json
```

## Docker イメージの詳細

### ベースイメージ

- **Ubuntu 22.04 LTS**
- 安定性と最新の依存関係のバランスが良い

### インストールされる依存関係

#### ビルドツール

- gcc, g++, make
- autoconf, automake
- CMake
- pkg-config

#### 必須ライブラリ

- zlib
- libpng
- FreeType
- bzip2
- SDL2

#### オプションライブラリ

- zstd
- miniUPnPc
- FluidSynth
- SDL2_mixer
- fontconfig

## トラブルシューティング

### コンテナが起動しない

**症状**: Docker Desktop が動作していない

**解決方法**:

```powershell
# Docker Desktopを起動
# Windowsの場合、スタートメニューから「Docker Desktop」を起動
```

### ビルドが失敗する

**症状**: メモリ不足エラー

**解決方法**:

```bash
# 並列ビルド数を減らす
docker-compose run --rm simutrans-build bash
# コンテナ内:
make -j2  # 2並列に制限
```

### ファイルの権限エラー（Linux）

**症状**: ビルド成果物が root ユーザー所有になる

**解決方法**:

```bash
# コンテナ実行後、権限を修正
sudo chown -R $USER:$USER build/

# または、Dockerfileを修正してユーザーIDを合わせる
```

### ディスク容量不足

**症状**: イメージビルドが失敗

**解決方法**:

```bash
# 不要なイメージとコンテナを削除
docker system prune -a

# ビルドキャッシュをクリア
docker builder prune
```

## アプリケーション側の修正

Docker 環境でのビルドを実現するため、以下の修正が Simutrans のソースコードに加えられました。

### 1. Dockerfile に libexpat1-dev を追加

fontconfig が libexpat に依存しているため、静的リンク時にシンボルが解決されないエラーが発生していました。

**修正内容**:

```dockerfile
RUN apt-get update && apt-get install -y \
    ...
    libexpat1-dev \  # 追加
    ...
```

**理由**: fontconfig ライブラリが XML パース機能で libexpat を使用しており、明示的にインストールする必要があります。

### 2. configure.ac での fontconfig 検索時に -lexpat を追加

fontconfig を検索する際に、依存関係である libexpat を明示的にリンクするよう修正しました。

**修正内容**:

```autoconf
# fontconfig オプション検索
AC_SEARCH_LIBS(FcInitLoadConfigAndFonts, fontconfig,
    [AC_SUBST(fontconfig, 1)],
    [AC_SUBST(fontconfig, 0)],
    [-lfontconfig -lexpat])  # 修正: -lexpat を追加
```

**理由**: configure スクリプトが fontconfig ライブラリをテストする際に、その依存関係も含めてリンクする必要があります。

### 3. Makefile での静的リンク時のオプション修正

Linux 環境では `pkg-config --libs --static` を使用して、すべての依存関係を正しく解決するよう修正しました。

**修正内容**:

```makefile
ifdef USE_FONTCONFIG
  ifeq ($(shell expr $(USE_FONTCONFIG) \>= 1), 1)
    CFLAGS  += -DUSE_FONTCONFIG
    CFLAGS  += $(shell $(FONTCONFIG_CONFIG) --cflags)
    ifeq ($(shell expr $(STATIC) \>= 1), 1)
      LDFLAGS += $(shell $(FONTCONFIG_CONFIG) --libs --static)  # 修正
    else
      LDFLAGS += $(shell $(FONTCONFIG_CONFIG) --libs)
    endif
  endif
endif
```

**理由**: 静的リンク時に、pkg-config の `--static` フラグを使用することで、libexpat を含むすべての依存ライブラリが正しくリンクされます。

### 4. config.default.in での STATIC フラグを 0 に変更

Linux 環境では完全な静的リンクが推奨されないため、デフォルト値を変更しました。

**修正内容**:

```makefile
# Before:
STATIC := 1   # Enable static linkage

# After:
STATIC := 0   # Enable static linkage (disabled for Linux)
```

**理由**:

- Linux では glibc の動的リンク機能（`_dl_x86_cpu_features` など）に依存しており、完全な静的リンクは問題を引き起こします
- 動的リンクを使用することで、メンテナンス性と互換性が向上します
- Docker 環境では、必要なライブラリがすべてコンテナ内に含まれるため、静的リンクの利点が限定的です

### 修正の検証

以下のコマンドで正常にビルドできることを確認できます：

```bash
docker-compose build
docker-compose run --rm simutrans-build docker-build.sh make

# 実行ファイルが生成されたことを確認
docker-compose run --rm simutrans-build ls -lh /simutrans/sim
```

成功時の出力例:

```
-rwxr-xr-x 1 root root 7.9M Jan 12 08:22 /simutrans/sim
```

## 高度な使い方

### カスタムビルド引数

```bash
# 特定のバックエンドでビルド
docker-compose run --rm simutrans-build bash
# コンテナ内:
cmake -B build -DSIMUTRANS_BACKEND=gdi
cmake --build build
```

### マルチステージビルド

実行環境を軽量化したい場合は、Dockerfile をマルチステージに修正：

```dockerfile
# ビルドステージ
FROM ubuntu:22.04 AS builder
# ... ビルド処理 ...

# 実行ステージ
FROM ubuntu:22.04
COPY --from=builder /simutrans/build/default/sim /usr/local/bin/
# 実行時の依存関係のみインストール
RUN apt-get update && apt-get install -y \
    libsdl2-2.0-0 \
    libpng16-16 \
    ...
```

### 複数のコンテナを同時実行

```bash
# バックグラウンドで起動
docker-compose up -d simutrans-build

# 別のターミナルから接続
docker-compose exec simutrans-build bash
```

## CI/CD 統合

### GitHub Actions

```yaml
name: Build Simutrans

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build Docker image
        run: docker-compose build
      - name: Build Simutrans
        run: docker-compose run --rm simutrans-build docker-build.sh make
```

### GitLab CI

```yaml
build:
  image: docker:latest
  services:
    - docker:dind
  script:
    - docker-compose build
    - docker-compose run --rm simutrans-build docker-build.sh make
```

## パフォーマンス最適化

### ビルドキャッシュの活用

```bash
# Dockerのビルドキャッシュを使用
docker-compose build --no-cache  # キャッシュをクリアして再ビルド

# ボリュームでビルドキャッシュを永続化（docker-compose.ymlで設定済み）
```

### 並列ビルド数の調整

```bash
# CPUコア数に応じて調整
docker-compose run --rm -e MAKEFLAGS="-j8" simutrans-build docker-build.sh make
```

## セキュリティ考慮事項

### 非 root ユーザーの使用

本番環境では、非 root ユーザーでコンテナを実行することを推奨：

```dockerfile
# Dockerfileに追加
RUN useradd -m -u 1000 simutrans
USER simutrans
```

### イメージの定期更新

```bash
# ベースイメージを定期的に更新
docker-compose build --pull
```

## 他のドキュメント

- [開発環境セットアップ](DEVELOPMENT_SETUP.md) - ネイティブ環境でのセットアップ
- [技術アーキテクチャ](TECHNICAL_ARCHITECTURE.md) - コードベースの理解
- [依存関係](DEPENDENCIES.md) - 依存ライブラリの詳細

## 参考リンク

- [Docker Documentation](https://docs.docker.com/)
- [Docker Compose Documentation](https://docs.docker.com/compose/)
- [VSCode Dev Containers](https://code.visualstudio.com/docs/remote/containers)
