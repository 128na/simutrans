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
