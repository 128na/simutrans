#!/bin/bash
# Simutrans Docker Build Script

set -e  # エラーで停止

echo "=================================="
echo "Simutrans Docker Build Script"
echo "=================================="
echo ""

# ビルドタイプを引数から取得（デフォルト: make）
BUILD_TYPE="${1:-make}"

case "$BUILD_TYPE" in
  make)
    echo "Building with Make..."

    # configureが存在しない場合は生成
    if [ ! -f "configure" ]; then
      echo "Running autoconf..."
      autoconf
    fi

    # configureを実行
    if [ ! -f "config.default" ]; then
      echo "Running configure..."
      ./configure
    fi

    # ビルド
    echo "Building Simutrans..."
    make -j$(nproc)

    echo ""
    echo "Build complete!"
    echo "Executable: build/default/sim"
    ;;

  cmake)
    echo "Building with CMake..."

    # ビルドディレクトリを作成
    mkdir -p build
    cd build

    # CMake設定
    echo "Running CMake configuration..."
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DSIMUTRANS_BACKEND=sdl2 \
          ..

    # ビルド
    echo "Building Simutrans..."
    cmake --build . -j$(nproc)

    cd ..
    echo ""
    echo "Build complete!"
    echo "Executable: build/simutrans/sim"
    ;;

  clean)
    echo "Cleaning build artifacts..."

    # Makeのクリーン
    if [ -f "Makefile" ]; then
      make clean || true
    fi

    # CMakeのクリーン
    if [ -d "build" ]; then
      rm -rf build
    fi

    # 生成ファイルの削除
    rm -f config.default config.log

    echo "Clean complete!"
    ;;

  *)
    echo "Usage: docker-build.sh [make|cmake|clean]"
    echo ""
    echo "Options:"
    echo "  make   - Build using Make (default)"
    echo "  cmake  - Build using CMake"
    echo "  clean  - Clean build artifacts"
    exit 1
    ;;
esac
