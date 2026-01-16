# Simutrans Build Environment
# Ubuntu 22.04ベースのビルド環境

FROM ubuntu:22.04

# タイムゾーン設定（インタラクティブプロンプトを回避）
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Tokyo

# 基本パッケージと依存関係のインストール
RUN apt-get update && apt-get install -y \
    # ビルドツール
    build-essential \
    autoconf \
    automake \
    pkg-config \
    cmake \
    git \
    subversion \
    # 必須ライブラリ
    zlib1g-dev \
    libpng-dev \
    libfreetype6-dev \
    libbz2-dev \
    libsdl2-dev \
    libexpat1-dev \
    # オプションライブラリ
    libzstd-dev \
    libminiupnpc-dev \
    libfluidsynth-dev \
    libsdl2-mixer-dev \
    libfontconfig1-dev \
    # ユーティリティ
    wget \
    curl \
    unzip \
    vim \
    nano \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# 作業ディレクトリを設定
WORKDIR /simutrans

# ビルド用のスクリプトをコピー（オプション）
COPY docker-build.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/docker-build.sh

# ビルド時のデフォルトコマンド
CMD ["/bin/bash"]
