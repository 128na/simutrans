# Simutrans 開発者向けドキュメント（トップ）

## 概要

このディレクトリは、Simutrans プロジェクトの技術ドキュメント（日本語）をまとめたトップページです。詳細な一覧は全ページ索引に集約しました。カテゴリ別の入口のみを簡潔に掲載します。

**対象読者:**

- 初心者向けガイドを読みたい方
- システムの内部構造を理解したい開発者
- 機能を拡張したいコントリビューター

## 目的

- カテゴリ別の入口を提供し、導線を簡潔化
- 詳細な一覧は索引ページ（全リンク集）へ集約
- ドキュメントの保守性・可読性を向上

## 基本的な使用方法

- まずは「📚 索引（全ページ）」から目的のドキュメントへ移動してください。
- 主要カテゴリのクイックリンクも用意しています。

---

## クイックリンク

- 📚 全ページ索引: [ALL_PAGES.md](ALL_PAGES.md)
- 🚀 入門・セットアップ: [guides/](guides/)
- 🏗️ アーキテクチャ: [architecture/](architecture/)
- 🎮 個別システム: [systems/](systems/)

---

## 関連ファイル

- 索引ページ: `docs/ALL_PAGES.md`
- ガイド: `docs/guides/README.md`
- アーキテクチャ: `docs/architecture/README.md`
- システム: `docs/systems/README.md`

## まとめ

README はカテゴリの入口のみを提供し、詳細は [ALL_PAGES.md](ALL_PAGES.md) に集約しました。新しいドキュメントを追加した際は、カテゴリ側 README と索引の両方にリンクを追加してください。

### 目的別ガイド

#### 環境構築したい

→ [guides/DEVELOPMENT_SETUP.md](guides/DEVELOPMENT_SETUP.md) または [guides/DOCKER_BUILD.md](guides/DOCKER_BUILD.md)

#### 機能追加・改善したい

→ [architecture/FEATURES.md](architecture/FEATURES.md) で該当システムを確認

#### バグ修正したい

→ [architecture/TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md) でコンポーネント構造を理解

#### サーバー管理したい

→ [systems/NETTOOL.md](systems/NETTOOL.md) でサーバー制御を学習

#### Pakset を作成したい

→ [systems/MAKEOBJ.md](systems/MAKEOBJ.md) で makeobj ツールを学習

#### シナリオ・ミッションを作成したい

→ [systems/SCENARIO.md](systems/SCENARIO.md) でシナリオシステムを理解

#### Steam Workshop でコンテンツを公開したい

→ [systems/STEAM_WORKSHOP.md](systems/STEAM_WORKSHOP.md) で Workshop 統合を学習

---

## 🚀 クイックスタート

### Docker で始めたい方（最も簡単！）🐋

1. **[DOCKER_BUILD.md](guides/DOCKER_BUILD.md)** に従って Docker 環境を構築
2. すぐにビルド開始：
   ```bash
   docker-compose run --rm simutrans-build docker-build.sh make
   ```
3. コードを読む前に **[TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md)** でアーキテクチャを学習

### 完全な初心者向け

1. **[PROJECT_OVERVIEW.md](guides/PROJECT_OVERVIEW.md)** を読んでプロジェクトを理解
2. **[DEVELOPMENT_SETUP.md](guides/DEVELOPMENT_SETUP.md)** に従って環境を構築
3. コードを読み始める前に **[TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md)** でアーキテクチャを学習

### 既に開発経験がある方

1. **[DEVELOPMENT_SETUP.md](guides/DEVELOPMENT_SETUP.md)** で環境を素早く構築
2. **[PROJECT_OVERVIEW.md](guides/PROJECT_OVERVIEW.md)** のディレクトリ構成を確認
3. **[TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md)** で関心のある部分を参照

### 依存関係の問題が発生した場合

1. **[DEPENDENCIES.md](guides/DEPENDENCIES.md)** のトラブルシューティングセクションを確認
2. それでも解決しない場合は [フォーラム](https://forum.simutrans.com/) で質問

---

## 🎓 推奨学習パス

```bash
# ドキュメントを読む
cd docs
# guides/PROJECT_OVERVIEW.md → guides/DEVELOPMENT_SETUP.md → architecture/TECHNICAL_ARCHITECTURE.md

# 環境を構築
# Windows: Visual Studio を使うか MSYS2 を使う
# Linux/Mac: 通常の make/CMake を使う
# Docker: docker-compose を使う（推奨）

# コードを読む
cd ../src/simutrans

# デバッガでステップ実行しながら理解を深める
```

### 学習ステップ

1. **基礎理解（1〜2 週間）**

   - プロジェクト概要とアーキテクチャを学習
   - ビルド環境を整備
   - デバッガの使い方を習得

2. **コア機能の理解（2〜4 週間）**

   - シミュレーションループ（`simworld.cc`）
   - 描画システム（`display.cc`）
   - 基本オブジェクト（`simobj.cc`）

3. **特定システムの深掘り（1〜2 ヶ月）**

   - 興味のあるコンポーネント（vehicle/、gui/、dataobj/ など）を選択
   - 小さな変更やバグ修正にトライ

4. **実践貢献（継続的）**
   - パッチの作成と投稿
   - フォーラムでのディスカッション参加

---

## 🔍 コードを読む際のヒント

1. **デバッガを活用する**
   Visual Studio、VS Code、GDB などでブレークポイントを設定し、ステップ実行しながら理解を深めましょう。

2. **ディレクトリごとに分割して理解する**
   一度に全てを理解しようとせず、興味のあるディレクトリ（例：`vehicle/`）から始めましょう。

3. **データ構造を可視化する**
   `simworld.h`、`simobj.h`、`vehicle.h` などの主要ヘッダーファイルでクラス構造を確認しましょう。

4. **ゲームをプレイしながらコードを読む**
   実際のゲーム内動作と対応するコードを関連付けると理解が深まります。

5. **フォーラムで質問する**
   わからないことがあれば、[Simutrans Forum](https://forum.simutrans.com/) で質問しましょう。

---

## 📦 プロジェクト構成（簡易版）

```
simutrans/
├── src/               # ソースコード（C++）
│   ├── simutrans/     # メインシミュレーションコード
│   ├── makeobj/       # Pak 作成ツール
│   ├── nettool/       # ネットワークツール
│   └── squirrel/      # スクリプトエンジン（Squirrel）
├── simutrans/         # ゲームデータ（テキスト、スクリプト）
├── cmake/             # CMake ビルド設定
├── docs/              # 日本語ドキュメント（このディレクトリ）
└── documentation/     # 英語ドキュメント
```

詳細は **[PROJECT_OVERVIEW.md](guides/PROJECT_OVERVIEW.md)** を参照してください。

---

## 🛠️ 開発ワークフロー

### 1. Issue の確認

- [GitHub Issues](https://github.com/simutrans/simutrans/issues) をチェック
- [フォーラム](https://forum.simutrans.com/index.php/board,33.0.html) でディスカッション

### 2. コードの変更

- ローカル環境で変更を加える
- デバッガでテスト
- コーディングスタイルを守る（`documentation/coding_styles.txt`）

### 3. ビルドとテスト

```bash
# ビルド
make clean && make

# テスト実行
./build/default/simutrans/simutrans
```

### 4. パッチの作成

```bash
# パッチファイルを作成
git diff > my_changes.patch
```

### 5. パッチの投稿

- [Patches & Projects フォーラム](https://forum.simutrans.com/index.php/board,33.0.html) にパッチを投稿
- 変更内容と目的を説明

### 6. 貢献

- **Pull Request は使用しない**
- [Patches & Projects フォーラム](https://forum.simutrans.com/index.php/board,33.0.html)にパッチを投稿

---

## ❓ よくある質問（FAQ）

### Q: どこから始めればいい？

**A:** [PROJECT_OVERVIEW.md](guides/PROJECT_OVERVIEW.md)を読んで全体像を把握し、[DEVELOPMENT_SETUP.md](guides/DEVELOPMENT_SETUP.md)で環境を構築してください。

### Q: コードが大きすぎて圧倒される

**A:** [TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md)でアーキテクチャを理解し、興味のあるコンポーネント（例：vehicle/、gui/）から始めてください。

### Q: ビルドエラーが出る

**A:** [DEPENDENCIES.md](guides/DEPENDENCIES.md)のトラブルシューティングセクションを確認するか、[フォーラム](https://forum.simutrans.com/)で質問してください。

### Q: Pull Request を送りたい

**A:** Simutrans プロジェクトは Pull Request を使用していません。パッチファイル（`.patch`）を作成して、[フォーラム](https://forum.simutrans.com/index.php/board,33.0.html)に投稿してください。

### Q: ドキュメントに誤りを見つけた

**A:** GitHub で Issue を作成するか、フォーラムで報告してください。

---

## 📞 サポート・コミュニティ

- **公式フォーラム**: https://forum.simutrans.com/
- **GitHub リポジトリ**: https://github.com/simutrans/simutrans
- **公式 Wiki**: https://simutrans-germany.com/wiki/
- **Discord**: [Simutrans Discord](https://discord.gg/C4Fp7jb)

---

## 🚀 次のステップ

1. ✅ この README.md を読む
2. 📖 [PROJECT_OVERVIEW.md](guides/PROJECT_OVERVIEW.md) でプロジェクトを理解
3. 🛠️ [DEVELOPMENT_SETUP.md](guides/DEVELOPMENT_SETUP.md) で環境を構築
4. 🏗️ [TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md) でアーキテクチャを学習
5. ⚙️ [FEATURES.md](architecture/FEATURES.md) で主要機能を深掘り
6. 💻 コードを読み始める！

Happy Coding! 🚂
