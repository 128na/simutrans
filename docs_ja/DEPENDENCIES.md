# Simutrans 依存関係リファレンス

## 概要

このドキュメントでは、Simutrans のすべての依存ライブラリとその詳細を説明します。

## 必須依存関係

### zlib

- **バージョン**: 1.2.11 以降推奨
- **ライセンス**: zlib License
- **用途**: 基本的な圧縮サポート
- **URL**: https://zlib.net/

#### 使用箇所

- セーブファイルの圧縮
- ネットワークデータの圧縮
- Pakset ファイルの読み込み

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install zlib1g-dev
```

**Arch Linux:**

```bash
sudo pacman -S zlib
```

**macOS:**

```bash
brew install zlib
```

**Windows (vcpkg):**

```powershell
vcpkg install zlib:x64-windows-static
```

### libpng

- **バージョン**: 1.6.x
- **ライセンス**: libpng License
- **用途**: PNG 画像の読み込みと書き込み
- **URL**: http://www.libpng.org/pub/png/

#### 使用箇所

- ゲームグラフィックス（Pakset 内の PNG）
- スクリーンショット保存
- UI テクスチャ

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libpng-dev
```

**Arch Linux:**

```bash
sudo pacman -S libpng
```

**macOS:**

```bash
brew install libpng
```

**Windows (vcpkg):**

```powershell
vcpkg install libpng:x64-windows-static
```

### FreeType

- **バージョン**: 2.10.0 以降
- **ライセンス**: FreeType License (BSD-style)
- **用途**: TrueType フォントのレンダリング
- **URL**: http://www.freetype.org/

#### 使用箇所

- ゲーム内テキスト表示
- UI フォント
- 多言語サポート

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libfreetype6-dev
```

**Arch Linux:**

```bash
sudo pacman -S freetype2
```

**macOS:**

```bash
brew install freetype
```

**Windows (vcpkg):**

```powershell
vcpkg install freetype:x64-windows-static
```

### bzip2

- **バージョン**: 1.0.6 以降
- **ライセンス**: BSD-style license
- **用途**: 代替圧縮アルゴリズム（zstd と排他）
- **URL**: https://www.bzip.org/downloads.html

#### 使用箇所

- セーブファイルの圧縮（zstd より高圧縮率）
- ネットワークデータ

**注意**: bzip2 と zstd のどちらか一方が必要

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libbz2-dev
```

**Arch Linux:**

```bash
sudo pacman -S bzip2
```

**macOS:**

```bash
brew install bzip2
```

**Windows (vcpkg):**

```powershell
vcpkg install bzip2:x64-windows-static
```

### SDL2 (条件付き必須)

- **バージョン**: 2.0.8 以降
- **ライセンス**: zlib License
- **用途**: クロスプラットフォームグラフィックス＆入力
- **URL**: http://www.libsdl.org/

#### プラットフォーム要件

- **Linux**: 必須
- **macOS**: 必須
- **Windows**: オプション（ただし強く推奨）

#### 使用箇所

- ウィンドウ管理
- グラフィックス描画
- キーボード/マウス入力
- サウンド再生（SDL2_mixer 使用時）

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libsdl2-dev
```

**Arch Linux:**

```bash
sudo pacman -S sdl2
```

**macOS:**

```bash
brew install sdl2
```

**Windows (vcpkg):**

```powershell
vcpkg install sdl2:x64-windows-static
```

## オプション依存関係

### zstd (Zstandard)

- **バージョン**: 1.4.0 以降
- **ライセンス**: BSD License
- **用途**: 高速圧縮（bzip2 の代替）
- **URL**: https://github.com/facebook/zstd

#### 利点

- bzip2 より高速
- 良好な圧縮率
- マルチスレッド対応

#### 欠点

- ファイルサイズが bzip2 より大きい

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libzstd-dev
```

**Arch Linux:**

```bash
sudo pacman -S zstd
```

**macOS:**

```bash
brew install zstd
```

**Windows (vcpkg):**

```powershell
vcpkg install zstd:x64-windows-static
```

### miniUPnPc

- **バージョン**: 2.0 以降
- **ライセンス**: BSD License
- **用途**: 自動ポートフォワーディング（サーバー機能）
- **URL**: http://miniupnp.free.fr/

#### 機能

- UPnP による自動ポート開放
- NAT traversal
- 簡易サーバー設定

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libminiupnpc-dev
```

**Arch Linux:**

```bash
sudo pacman -S miniupnpc
```

**macOS:**

```bash
brew install miniupnpc
```

**Windows (vcpkg):**

```powershell
vcpkg install miniupnpc:x64-windows-static
```

### FluidSynth

- **バージョン**: 2.0 以降
- **ライセンス**: LGPL
- **用途**: MIDI 音楽再生
- **URL**: https://www.fluidsynth.org/

#### 推奨環境

- Linux: 推奨
- macOS: 一時的に推奨
- Windows: オプション（SDL2_mixer の方が簡単）

#### 機能

- 高品質 MIDI 合成
- SoundFont 対応
- 低レイテンシー

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libfluidsynth-dev
```

**Arch Linux:**

```bash
sudo pacman -S fluidsynth
```

**macOS:**

```bash
brew install fluid-synth
```

**注意**: SoundFont ファイル（.sf2）も別途必要

### SDL2_mixer

- **バージョン**: 2.0.4 以降
- **ライセンス**: zlib License
- **用途**: 代替 MIDI 再生＆サウンドシステム
- **URL**: http://www.libsdl.org/projects/SDL_mixer/

#### 機能

- MIDI 再生
- 効果音ミキシング
- 複数フォーマット対応（OGG, MP3, WAV）

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libsdl2-mixer-dev
```

**Arch Linux:**

```bash
sudo pacman -S sdl2_mixer
```

**macOS:**

```bash
brew install sdl2_mixer
```

**Windows (vcpkg):**

```powershell
vcpkg install sdl2-mixer:x64-windows-static
```

### fontconfig

- **バージョン**: 2.13 以降
- **ライセンス**: MIT License
- **用途**: システムフォントの自動検出
- **URL**: https://www.fontconfig.org/

#### プラットフォーム

- Linux: 推奨
- macOS: 推奨
- Windows: 不要（独自実装）

#### 機能

- システムフォントの自動検索
- フォールバックフォント選択
- 多言語フォント対応

#### インストール

**Ubuntu/Debian:**

```bash
sudo apt-get install libfontconfig1-dev
```

**Arch Linux:**

```bash
sudo pacman -S fontconfig
```

**macOS:**

```bash
brew install fontconfig
```

## 開発ツール依存関係

### ビルドツール

#### autoconf

- **バージョン**: 2.69 以降
- **用途**: configure スクリプト生成
- **必要性**: Make 使用時のみ

**インストール:**

```bash
# Ubuntu/Debian
sudo apt-get install autoconf

# Arch Linux
sudo pacman -S autoconf

# macOS
brew install autoconf
```

#### CMake

- **バージョン**: 3.16 以降
- **用途**: クロスプラットフォームビルドシステム
- **必要性**: CMake 使用時のみ

**インストール:**

```bash
# Ubuntu/Debian
sudo apt-get install cmake

# Arch Linux
sudo pacman -S cmake

# macOS
brew install cmake

# Windows
# CMake公式サイトからインストーラーをダウンロード
```

#### pkg-config

- **用途**: ライブラリの検出と設定
- **必要性**: Linux/macOS

**インストール:**

```bash
# Ubuntu/Debian
sudo apt-get install pkg-config

# Arch Linux
sudo pacman -S pkgconf

# macOS
brew install pkg-config
```

### コンパイラ

#### GCC

- **バージョン**: 7.0 以降（C++14 サポート必須）
- **推奨**: 9.0 以降

**インストール:**

```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# Arch Linux
sudo pacman -S base-devel

# macOS
xcode-select --install
```

#### Clang

- **バージョン**: 5.0 以降
- **推奨**: 9.0 以降

**インストール:**

```bash
# Ubuntu/Debian
sudo apt-get install clang

# Arch Linux
sudo pacman -S clang

# macOS（デフォルト）
xcode-select --install
```

#### MSVC

- **バージョン**: Visual Studio 2017 以降
- **推奨**: Visual Studio 2019 以降
- **C++14 サポート必須**

**ダウンロード:**

- https://visualstudio.microsoft.com/

## vcpkg 統合（Windows）

### vcpkg とは

- Microsoft によるクロスプラットフォームパッケージマネージャー
- Windows での依存関係管理を簡素化
- CMake と統合

### vcpkg.json

プロジェクトルートに配置：

```json
{
	"version-string": "1.0.0",
	"dependencies": ["freetype", "miniupnpc", "pthread", "zstd", "sdl2", "zlib"],
	"builtin-baseline": "3c76dc55f8bd2b7f4824bcd860055094bfbbb9ea"
}
```

### 自動インストール

CMake と vcpkg を使用する場合、依存関係は自動的にビルド・インストールされます：

```powershell
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

## 依存関係マトリックス

### プラットフォーム別必須ライブラリ

| ライブラリ | Linux    | macOS    | Windows (SDL2) | Windows (GDI) |
| ---------- | -------- | -------- | -------------- | ------------- |
| zlib       | ✓        | ✓        | ✓              | ✓             |
| libpng     | ✓        | ✓        | ✓              | ✓             |
| FreeType   | ✓        | ✓        | ✓              | ✓             |
| bzip2/zstd | ✓        | ✓        | ✓              | ✓             |
| SDL2       | ✓        | ✓        | ✓              | ✗             |
| pthread    | システム | システム | vcpkg          | システム      |

### バックエンド別依存関係

| バックエンド | 必要なライブラリ       | プラットフォーム   |
| ------------ | ---------------------- | ------------------ |
| SDL2         | SDL2, SDL2_mixer (opt) | すべて             |
| GDI          | -                      | Windows 専用       |
| Server       | -                      | すべて（GUI 無し） |

### オプションライブラリ機能マトリックス

| 機能               | なし | bzip2    | zstd   |
| ------------------ | ---- | -------- | ------ |
| セーブファイル圧縮 | zlib | 高圧縮率 | 高速   |
| ファイルサイズ     | 基準 | 小さい   | 中程度 |
| 圧縮速度           | 基準 | 遅い     | 速い   |
| 解凍速度           | 基準 | 中程度   | 速い   |

| 機能         | なし | FluidSynth | SDL2_mixer |
| ------------ | ---- | ---------- | ---------- |
| MIDI 再生    | ✗    | 高品質     | 標準       |
| 効果音       | ✗    | -          | ✓          |
| セットアップ | 簡単 | 複雑       | 簡単       |
| SoundFont    | -    | ✓          | ✓          |

## ライセンス互換性

すべての依存ライブラリは、Simutrans のライセンス（Artistic License 1.0）と互換性があります：

| ライブラリ | ライセンス       | 互換性       |
| ---------- | ---------------- | ------------ |
| zlib       | zlib License     | ✓ OSI 承認   |
| libpng     | libpng License   | ✓ OSI 承認   |
| FreeType   | FreeType License | ✓ BSD-style  |
| bzip2      | BSD-style        | ✓ OSI 承認   |
| SDL2       | zlib License     | ✓ OSI 承認   |
| zstd       | BSD License      | ✓ OSI 承認   |
| miniUPnPc  | BSD License      | ✓ OSI 承認   |
| FluidSynth | LGPL             | ✓ 動的リンク |
| SDL2_mixer | zlib License     | ✓ OSI 承認   |
| fontconfig | MIT License      | ✓ OSI 承認   |

**注意**: FluidSynth は LGPL ですが、動的リンクにより使用可能です。

## トラブルシューティング

### ライブラリが見つからない

**症状**: `configure`や`cmake`が依存関係を検出できない

**解決方法**:

1. **pkg-config のパスを確認**:

   ```bash
   pkg-config --variable pc_path pkg-config
   ```

2. **ライブラリが実際にインストールされているか確認**:

   ```bash
   dpkg -l | grep libsdl2  # Ubuntu/Debian
   pacman -Q | grep sdl2   # Arch Linux
   brew list | grep sdl2   # macOS
   ```

3. **開発パッケージがインストールされているか確認**:
   - ヘッダーファイル（-dev または-devel パッケージ）が必要

### バージョン競合

**症状**: 異なるバージョンのライブラリが競合

**解決方法**:

1. **インストール済みバージョンを確認**:

   ```bash
   pkg-config --modversion sdl2
   ```

2. **古いバージョンを削除**:

   ```bash
   sudo apt-get remove --purge libsdl2-dev
   sudo apt-get autoremove
   ```

3. **再インストール**:
   ```bash
   sudo apt-get update
   sudo apt-get install libsdl2-dev
   ```

### リンクエラー

**症状**: ビルドは成功するが実行時にライブラリが見つからない

**解決方法**:

1. **共有ライブラリのパスを確認** (Linux):

   ```bash
   ldd ./build/default/sim
   ```

2. **LD_LIBRARY_PATH を設定**:

   ```bash
   export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
   ```

3. **ldconfig を実行** (要 root 権限):
   ```bash
   sudo ldconfig
   ```

### Windows 特有の問題

**症状**: vcpkg がライブラリをビルドできない

**解決方法**:

1. **vcpkg を最新に更新**:

   ```powershell
   cd [vcpkg-root]
   git pull
   .\bootstrap-vcpkg.bat
   ```

2. **キャッシュをクリア**:

   ```powershell
   .\vcpkg remove --outdated
   .\vcpkg install [package]:x64-windows-static --recurse
   ```

3. **マニフェストモードを使用**:
   - vcpkg.json をプロジェクトルートに配置
   - CMake で自動管理

## 参考リンク

### 公式ドキュメント

- [Simutrans Wiki - Compiling](https://simutrans-germany.com/wiki/wiki/en_CompilingSimutrans)
- [vcpkg Documentation](https://vcpkg.io/en/docs/)
- [CMake Documentation](https://cmake.org/documentation/)

### ライブラリドキュメント

- [SDL2 Wiki](https://wiki.libsdl.org/)
- [FreeType Documentation](https://www.freetype.org/freetype2/docs/documentation.html)
- [FluidSynth Documentation](https://www.fluidsynth.org/api/)

## 更新履歴

このドキュメントは定期的に更新されます。最新情報は以下で確認してください：

- [Simutrans Forum](https://forum.simutrans.com/)
- [GitHub Repository](https://github.com/simutrans/simutrans)
