# Simutrans 開発者向けドキュメント

このディレクトリには、Simutrans プロジェクトの技術スタック、アーキテクチャ、開発環境に関するドキュメントが含まれています。

## 📚 ドキュメント一覧

### [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)

**プロジェクト全体の概要**

Simutrans プロジェクトの基本情報、技術スタック、構造を説明しています。

**内容:**

- プロジェクトの背景と歴史
- 使用している技術スタック（言語、ビルドシステム、ライブラリ）
- ディレクトリ構成
- ビルド方法の概要
- 貢献方法
- 関連リンク

**対象読者:** 初めて Simutrans のコードに触れる開発者

---

### [TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md)

**技術アーキテクチャの詳細**

Simutrans の内部アーキテクチャとコードの構造を深く掘り下げています。

**内容:**

- システムアーキテクチャ概要
- コアコンポーネント（ワールド、オブジェクト、車両など）
- グラフィックス・UI・ネットワークシステム
- データ管理と I/O
- パフォーマンス最適化戦略
- 拡張性とモジュール性

**対象読者:** コードベースを理解したい開発者、機能を実装する開発者

---

### [DEVELOPMENT_SETUP.md](DEVELOPMENT_SETUP.md)

**開発環境セットアップガイド**

各プラットフォームでの開発環境の構築手順を詳しく説明しています。

**内容:**

- Windows（Visual Studio / MSYS2）
- Linux（Ubuntu/Debian、Arch、Fedora）
- macOS
- CMake の使用方法
- IDE 設定（VS Code、Visual Studio、CLion）
- トラブルシューティング

**対象読者:** 開発環境を構築したい開発者

---

### [DEPENDENCIES.md](DEPENDENCIES.md)

**依存関係リファレンス**

すべての依存ライブラリの詳細情報とインストール方法を提供しています。

**内容:**

- 必須ライブラリ（zlib、libpng、FreeType、SDL2 など）
- オプションライブラリ（zstd、miniUPnPc、FluidSynth など）
- プラットフォーム別インストール手順
- vcpkg 統合（Windows）
- 依存関係マトリックス
- ライセンス互換性
- トラブルシューティング

**対象読者:** ビルド環境を整備する開発者、依存関係の問題を解決したい開発者

---

### [DOCKER_BUILD.md](DOCKER_BUILD.md) 🐋

**Docker 環境でのビルド**

Docker を使用した一貫したビルド環境の構築方法を説明しています。

**内容:**

- Docker のインストール（Windows/Mac/Linux）
- Docker Compose を使用したビルド
- VSCode Dev Container 統合
- トラブルシューティング
- CI/CD 統合
- パフォーマンス最適化

**対象読者:** ホスト環境を汚さずにビルドしたい開発者、CI/CD 環境を構築したい開発者

**利点:**

- ✅ 依存関係の自動インストール
- ✅ クリーンな環境でのビルド
- ✅ クロスプラットフォーム対応
- ✅ CI/CD との統合が容易

---

### [NETTOOL.md](NETTOOL.md) 🛠️

**Nettool - ネットワークサーバー制御ツール**

Simutrans マルチプレイヤーサーバーをコマンドラインから管理・制御するユーティリティツールの完全ガイドです。

**内容:**

- nettool の概要と目的
- インストールとビルド方法
- 基本的な使用方法とオプション
- 全 15 個のコマンド詳細解説
  - サーバー情報・管理（announce、shutdown、force-sync）
  - クライアント管理（clients、kick-client、ban-client）
  - IP アドレス管理（ban-ip、unban-ip、blacklist）
  - 企業（会社）管理（companies、info-company、lock-company、unlock-company、remove-company）
  - メッセージング（say）
- 戻り値（リターンコード）
- 実用的な使用例とスクリプト
- セキュリティに関する注意事項
- トラブルシューティング
- 内部実装とプロトコル情報

**対象読者:**

- Simutrans 専用サーバーの管理者
- マルチプレイヤーゲームのホスティング運営者
- サーバー管理業務の自動化を検討している方

**主な機能:**

- 🎮 **クライアント管理**: プレイヤーの確認、キック、バン
- 🏢 **企業管理**: 企業情報確認、パスワード保護、削除
- 📢 **メッセージング**: 全プレイヤーへの通知配信
- 🔐 **セキュリティ**: IP アドレスのバン管理
- 🔧 **サーバー制御**: 同期強制、シャットダウン

---

### [VEHICLE_TERMINAL_SPEED.md](VEHICLE_TERMINAL_SPEED.md) 🚄

**重量と終端速度の関係**

車両（編成）の自重・積載が最高速度（終端速度）に与える影響を、コード式とともに整理しています。

**内容:**

- 残余パワー式と終端速度の決定方法
- `total_weight`（自重+積載）と `friction_weight` の積算方法
- カーブ/勾配による摩擦係数の変化
- 重量・積載が加速/最高速に与える実務的な読み替え

**対象読者:** 車両挙動やバランス調整を理解したい開発者

---

### [VEHICLE_ROUTE_SEARCH.md](VEHICLE_ROUTE_SEARCH.md) 🗺️

**車両の目的地検索（経路探索）ロジック**

車両タイプごとに異なる経路探索ロジックを、Mermaid 図とともに詳しく解説しています。

**内容:**

- 基本車両の経路探索（デフォルト実装）
- 道路車両の停留所予約と編成長チェック
- 鉄道車両の閉塞予約と信号制御
- 航空機の 3 段階経路（離陸 → 空中飛行 → 着陸）
- 船舶の JPS 最適化と水路判定
- 共通の A\*探索アルゴリズム
- 車両タイプ別カスタマイズポイント

**対象読者:**

- 経路探索アルゴリズムを理解したい開発者
- 車両挙動のカスタマイズを検討している方
- AI や経路システムの改善に取り組む方

**主な図解:**

- 📊 経路探索フローチャート（各車両タイプ）
- 🔄 目的地判定ロジック
- ✈️ 航空機の離着陸シーケンス
- 🧭 A\*探索アルゴリズムの詳細

---

## 🎯 クイックスタート

### Docker で始めたい方（最も簡単！）🐋

1. **[DOCKER_BUILD.md](DOCKER_BUILD.md)** に従って Docker 環境を構築
2. すぐにビルド開始：
   ```bash
   docker-compose run --rm simutrans-build docker-build.sh make
   ```
3. コードを読む前に **[TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md)** でアーキテクチャを学習

### 完全な初心者向け

1. **[PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)** を読んでプロジェクトを理解
2. **[DEVELOPMENT_SETUP.md](DEVELOPMENT_SETUP.md)** に従って環境を構築
3. コードを読み始める前に **[TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md)** でアーキテクチャを学習

### 既に開発経験がある方

1. **[DEVELOPMENT_SETUP.md](DEVELOPMENT_SETUP.md)** で環境を素早く構築
2. **[PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)** のディレクトリ構成を確認
3. **[TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md)** で関心のある部分を参照

### 依存関係の問題が発生した場合

1. **[DEPENDENCIES.md](DEPENDENCIES.md)** のトラブルシューティングセクションを確認
2. 該当するライブラリのインストール手順を再確認
3. それでも解決しない場合は[フォーラム](https://forum.simutrans.com/)で質問

---

## 📖 その他のドキュメント

### プロジェクトルートの documentation/ ディレクトリ

- **[about-the-code.txt](../documentation/about-the-code.txt)**: コードの基本構造と UI 設計
- **[coding_styles.txt](../documentation/coding_styles.txt)**: コーディング規約と命名規則
- **[utf8-support.txt](../documentation/utf8-support.txt)**: UTF-8 サポートの実装詳細

### オンラインリソース

#### Wiki

- [Compiling Simutrans](https://simutrans-germany.com/wiki/wiki/en_CompilingSimutrans)
- [Development Index](https://simutrans-germany.com/wiki/wiki/en_Devel_Index)
- [Doxygen Documentation](https://simutrans-germany.com/wiki/wiki/en_Doxygen)

#### フォーラム

- [Technical Documentation](https://forum.simutrans.com/index.php/board,112.0.html)
- [Patches & Projects](https://forum.simutrans.com/index.php/board,33.0.html)
- [Help Center](https://forum.simutrans.com/index.php/board,7.0.html)

---

## 🛠️ 開発ワークフロー

### オプション A: Docker を使用（推奨）🐋

```bash
# リポジトリをクローン
git clone https://github.com/simutrans/simutrans.git
cd simutrans

# Dockerイメージをビルド
docker-compose build

# ビルド実行
docker-compose run --rm simutrans-build docker-build.sh make

# または、VSCode Dev Containerで開発
# VSCodeでフォルダを開き、"Reopen in Container"を選択
```

### オプション B: ネイティブ環境

#### 1. 準備

```bash
# リポジトリをクローン
git clone https://github.com/simutrans/simutrans.git
cd simutrans

# ドキュメントを読む
cd docs_ja
# PROJECT_OVERVIEW.md → DEVELOPMENT_SETUP.md → TECHNICAL_ARCHITECTURE.md
```

#### 2. 環境構築

```bash
# プラットフォーム別のセットアップ
# Linux:
sudo ./tools/setup-debian.sh
# macOS:
brew install [依存関係]
# Windows:
# Visual Studio + vcpkg
```

#### 3. ビルド

```bash
# Makeの場合
autoconf
./configure
make -j$(nproc)

# CMakeの場合
cmake -B build
cmake --build build -j$(nproc)
```

#### 4. 実行

```bash
# Paksetをダウンロード
./tools/get_pak.sh

# 実行
./build/default/sim
```

### 5. コード変更（共通）

### 5. コード変更

```bash
# コーディング規約を確認
cat documentation/coding_styles.txt

# アーキテクチャを理解
# docs/TECHNICAL_ARCHITECTURE.md を参照

# コードを編集
# テストビルド
make -j$(nproc)
```

### 6. 貢献

- **Pull Request は使用しない**
- [Patches & Projects フォーラム](https://forum.simutrans.com/index.php/board,33.0.html)にパッチを投稿

---

## 🔍 コードを読む際のヒント

### 主要なエントリーポイント

1. **src/simutrans/simmain.cc** - メイン関数、初期化処理
2. **src/simutrans/world/simworld.cc** - ワールド管理、メインループ
3. **src/simutrans/display/simgraph.h** - グラフィックス抽象レイヤー
4. **src/simutrans/gui/gui_frame_t.h** - UI 基底クラス

### コード探索の順序

1. **データ構造**: `src/simutrans/tpl/` でテンプレートライブラリを確認
2. **基本オブジェクト**: `src/simutrans/obj/` でゲームオブジェクトを理解
3. **システムコンポーネント**: 興味のあるシステム（vehicle/, network/, gui/など）を深掘り

### デバッグのコツ

```cpp
// simdebug.hを使用
#include "simdebug.h"

dbg->message("tag", "Debug message: %d", value);
dbg->warning("tag", "Warning!");
dbg->error("tag", "Error!");
```

---

## 💡 よくある質問

### Q: どこから始めればいい？

**A:** [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md)を読んで全体像を把握し、[DEVELOPMENT_SETUP.md](DEVELOPMENT_SETUP.md)で環境を構築してください。

### Q: コードが大きすぎて圧倒される

**A:** [TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md)でアーキテクチャを理解し、興味のあるコンポーネント（例：vehicle/、gui/）から始めてください。

### Q: ビルドエラーが出る

**A:** [DEPENDENCIES.md](DEPENDENCIES.md)のトラブルシューティングセクションを確認するか、[フォーラム](https://forum.simutrans.com/)で質問してください。

### Q: どうやって貢献できる？

**A:**

1. コーディング: [Patches & Projects](https://forum.simutrans.com/index.php/board,33.0.html)
2. 翻訳: [SimuTranslator](https://translator.simutrans.com/)
3. グラフィック: [General Resources](https://forum.simutrans.com/index.php/board,108.0.html)
4. バグ報告: [Bug Reports](https://forum.simutrans.com/index.php/board,8.0.html)

### Q: Pull Request は使えない？

**A:** いいえ、Simutrans プロジェクトはフォーラムベースの開発を行っています。パッチは[Patches & Projects フォーラム](https://forum.simutrans.com/index.php/board,33.0.html)に投稿してください。

---

## 🤝 コミュニティ

### コミュニケーション

- **国際フォーラム**: https://forum.simutrans.com/
- **ドイツフォーラム**: https://www.simutrans-forum.de/
- **日本フォーラム**: https://forum.japanese.simutrans.com/

### 開発チーム

- **リードメンテナ**: Markus Pristovsek "Prissi"
- **創始者**: Hansjörg Malthaner "Hajo" (1997-2004)
- **貢献者**: [thanks.txt](../simutrans/thanks.txt)を参照

### サポート

- **技術的な質問**: [Technical Documentation フォーラム](https://forum.simutrans.com/index.php/board,112.0.html)
- **一般的なヘルプ**: [Help Center](https://forum.simutrans.com/index.php/board,7.0.html)
- **バグ報告**: [Bug Reports](https://forum.simutrans.com/index.php/board,8.0.html)

---

## 📝 ライセンス

Simutrans は **Artistic License 1.0** の下で配布されています。

- ライセンス全文: [../LICENSE.txt](../LICENSE.txt)
- OSI 承認ライセンス
- 使用、配布、修正、修正版の配布が可能

**注意**: Pakset（ゲームデータ）は別ライセンスです。

---

## 🔄 ドキュメントの更新

このドキュメントは 2026 年 1 月時点の情報に基づいています。

### 更新履歴

- 2026-01-12: 初版作成

### 貢献

ドキュメントの改善提案は[フォーラム](https://forum.simutrans.com/)で歓迎します！

---

## 🚀 次のステップ

1. ✅ この README.md を読む
2. 📖 [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) でプロジェクトを理解
3. 🛠️ [DEVELOPMENT_SETUP.md](DEVELOPMENT_SETUP.md) で環境を構築
4. 🏗️ [TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md) でアーキテクチャを学習
5. 💻 コードを読み始める！

Happy Coding! 🚂
