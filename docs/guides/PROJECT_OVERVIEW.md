# Simutrans プロジェクト概要

## プロジェクトについて

Simutrans は、オープンソースの交通シミュレーションゲームです。鉄道、道路、船舶、航空機による旅客・郵便・貨物輸送を通じて、成功した運輸会社の経営を目指します。

- **ライセンス**: Artistic License 1.0
- **開発開始**: 1997 年（Hansjörg Malthaner 氏により）
- **現在の開発**: The Simutrans Team（Markus Pristovsek "Prissi"氏がリード）
- **公式サイト**: https://www.simutrans.com/

## 技術スタック

### プログラミング言語

- **C++14** (標準モード)
- **C++17** (Steam 版ビルド時)
- **C 言語** (一部のシステムレベルコード)

### ビルドシステム

プロジェクトは 3 つのビルドシステムをサポート：

1. **Make** (推奨: デバッグビルド、Linux)
2. **CMake 3.16+** (推奨: macOS、Android)
3. **Microsoft Visual Studio** (推奨: Windows、デバッグビルド)

### 依存ライブラリ

#### 必須ライブラリ

| ライブラリ                | バージョン | 用途                                                     |
| ------------------------- | ---------- | -------------------------------------------------------- |
| **zlib**                  | -          | 基本的な圧縮サポート                                     |
| **libpng**                | -          | 画像操作                                                 |
| **libfreetype**           | -          | TrueType フォントサポート                                |
| **bzip2** または **zstd** | -          | 代替圧縮（どちらか一方が必須。vcpkg では zstd を使用）   |
| **SDL2**                  | 2.x        | グラフィックバックエンド（Linux/Mac 必須、Windows 推奨） |

#### オプションライブラリ

| ライブラリ     | 用途                                    |
| -------------- | --------------------------------------- |
| **miniupnpc**  | 簡易サーバー機能                        |
| **fluidsynth** | MIDI 再生（Linux 推奨、Mac 一時的推奨） |
| **SDL2_mixer** | 代替 MIDI 再生とサウンドシステム        |
| **fontconfig** | フォント自動検出（Linux/Mac）           |

### グラフィックバックエンド

- **SDL2** (クロスプラットフォーム、推奨)
- **GDI** (Windows 専用)
- **Server** (GUI なし、サーバー専用)

## プロジェクト構造

### ディレクトリ構成

```
simutrans/
├── src/                      # ソースコード
│   ├── simutrans/           # メインゲームコード
│   │   ├── builder/         # 建設関連
│   │   ├── dataobj/         # データオブジェクト
│   │   ├── descriptor/      # 記述子
│   │   ├── display/         # 表示システム
│   │   ├── ground/          # 地形
│   │   ├── gui/             # ユーザーインターフェース
│   │   ├── io/              # 入出力
│   │   ├── network/         # ネットワーク機能
│   │   ├── obj/             # ゲームオブジェクト
│   │   ├── player/          # プレイヤー管理
│   │   ├── script/          # スクリプト（AI）
│   │   ├── sound/           # サウンドシステム
│   │   ├── sys/             # システム依存コード
│   │   ├── tool/            # ツール
│   │   ├── tpl/             # テンプレートライブラリ
│   │   ├── utils/           # ユーティリティ
│   │   ├── vehicle/         # 車両システム
│   │   └── world/           # ワールド管理
│   ├── makeobj/             # Pakset作成ツール
│   ├── nettool/             # ネットワークツール
│   ├── squirrel/            # Squirrelスクリプトエンジン
│   ├── android/             # Android固有コード
│   ├── linux/               # Linux固有コード
│   ├── OSX/                 # macOS固有コード
│   └── Windows/             # Windows固有コード
├── cmake/                    # CMakeビルドスクリプト
├── documentation/            # 開発者向けドキュメント
├── simutrans/               # ゲームデータ（AI、設定、テキストなど）
├── tests/                   # テストスクリプト
├── themes.src/              # UIテーマソース
└── tools/                   # 開発ツール

```

### Visual Studio プロジェクト構成

- **Simutrans-Main**: バックエンド非依存の共有コード
- **Simutrans SDL2**: SDL2 バックエンド（推奨）
- **Simutrans GDI**: GDI バックエンド（Windows 専用）
- **Simutrans Server**: サーバーバックエンド（GUI 無し）

## アーキテクチャ概要

### コア構造

Simutrans は階層的なオブジェクト指向設計を採用：

1. **karte_t** (マップ/世界)
   - 2D 座標の配列を保持（planquadrat_t）
2. **planquadrat_t** (2D 座標)
   - grund_t の配列を保持
3. **grund_t** (地面・基礎)
   - トンネル、橋、高速道路、水域などを表現
   - objlist を保持
4. **obj_t** (オブジェクト)
   - 車、列車、飛行機などの移動オブジェクト
   - 建物、木などの静的オブジェクト
   - obj/フォルダに配置

### 更新サイクル

オブジェクトは 3 種類の定期的な呼び出しを持つ：

- **sync_step()**: フレームごとに呼ばれる高速アクション
  - 排他的なマップアクセスが必要
  - 車の移動、衝突回避など
- **step()**: 比較的低速なアクション
  - ルート検索、道路建設など
  - 実行間でマップが変更される可能性あり
- **check_season()**: 季節変化
  - 木などが使用

### UI アーキテクチャ

- メインコードからほぼ完全に分離
- すべてのダイアログは`gui_frame_t`から派生
- コンポーネントベース設計（gui/components/）
- イベント駆動型（action_listener パターン）

## ビルド方法

### Make の場合（Linux/Mac）

```bash
# Linux
autoconf
./configure
make -j 4

# macOS
autoreconf -ivf
./configure
make -j 4
make OSX/getversion
```

### CMake の場合

```bash
# Linux/MinGW/macOS
cmake -B build .
cmake --build build -j 4

# MSVC
cmake.exe .. -G "Visual Studio 16 2019" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Visual Studio の場合

1. `Simutrans.sln`を開く
2. スタートアッププロジェクトを選択（SDL2/GDI/Server）
3. ビルド実行

## 開発環境セットアップ

### Windows

- **Visual Studio** または **MSYS2** をインストール
- vcpkg で依存ライブラリを自動インストール
  ```
  tools\install-building-libs-{architecture}.bat
  ```

### Linux (Ubuntu/Debian)

```bash
tools/setup-debian.sh
```

### Mac

- Homebrew で依存ライブラリをインストール

## コーディングスタイル

詳細は`documentation/coding_styles.txt`を参照

主要なルール：

- 最もシンプルな実装を使用
- 一貫した命名規則（同じものには同じ名前）
- 意味のある名前（メソッド名は動詞、クラス名は名詞）
- 可能な限り const を使用
- クラスメンバーはできるだけ private に
- すべてのメンバー変数をコンストラクタで初期化
- 標準プレフィックス:
  - `get_`: 値を返す
  - `set_`: 値を設定
  - `is_`: 条件をテスト

## バージョン管理

- **メインリポジトリ**: SVN (svn://servers.simutrans.org/simutrans/trunk)
- **ミラー**: GitHub (https://github.com/simutrans/simutrans)
  - Git ミラーを使用する場合、ネットワークゲーム参加時は手動でバージョン設定が必要

## 貢献方法

### コード貢献

- **Pull Request は使用しない**
- 代わりに[Patches & Projects](https://forum.simutrans.com/index.php/board,33.0.html)フォーラムを使用
- コーディングガイドラインに従う

### 翻訳

- [SimuTranslator](https://translator.simutrans.com/)ウェブツールを使用

### グラフィック

- [General Resources and Tools](https://forum.simutrans.com/index.php/board,108.0.html)フォーラムを参照

### バグ報告

- [Bug Reports](https://forum.simutrans.com/index.php/board,8.0.html)フォーラムを使用

## 関連リンク

- 公式サイト: https://www.simutrans.com/
- 国際フォーラム: https://forum.simutrans.com/
- Wiki: https://wiki.simutrans.com
- GitHub: https://github.com/simutrans/simutrans
- Steam: https://store.steampowered.com/app/434520/Simutrans/

## ライセンス

Artistic License 1.0（OSI 承認）。詳細は`LICENSE.txt`を参照。

Pakset（ゲーム実行に必要）は別ライセンス。
