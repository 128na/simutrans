# Simutrans 開発者向けドキュメント（非公式だよ ☆）

このページは [github.com/aburch/simutrans](https://github.com/aburch/simutrans) の内容を Copilot 大先生に日本語でいい感じに訳してもらったドキュメントです。
継続的にメンテナンスするかは未定です。内容が不正確・古い場合が多分にあるためご注意ください。

ドキュメントの元データはこちら [github.com/128na/simutrans](https://github.com/128na/simutrans/tree/misc/docs)

## ドキュメントの内容について

このディレクトリには、Simutrans プロジェクトの技術スタック、アーキテクチャ、開発環境に関するドキュメントが含まれています。

## 📂 ドキュメント構成

- **[guides/](guides/)** - 初心者向けガイド・環境構築
- **[architecture/](architecture/)** - アーキテクチャ・設計思想
- **[systems/](systems/)** - 個別システムの詳細解説

---

## 📚 ドキュメント一覧

### 🚀 入門ガイド（guides/）

#### [PROJECT_OVERVIEW.md](guides/PROJECT_OVERVIEW.md)

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

#### [DEVELOPMENT_SETUP.md](guides/DEVELOPMENT_SETUP.md)

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

#### [DEPENDENCIES.md](guides/DEPENDENCIES.md)

**依存関係リファレンス**

すべての依存ライブラリの詳細情報とインストール方法を提供しています。

**内容:**

- 必須ライブラリ（zlib、libpng、FreeType、SDL2 など）
- オプションライブラリ（zstd、miniUPNPc、FluidSynth など）
- プラットフォーム別インストール手順
- vcpkg 統合（Windows）
- 依存関係マトリックス
- ライセンス互換性
- トラブルシューティング

**対象読者:** ビルド環境を整備する開発者、依存関係の問題を解決したい開発者

---

#### [DOCKER_BUILD.md](guides/DOCKER_BUILD.md) 🐋

**Docker ビルド環境**

Docker を使用して Simutrans をビルドするための完全ガイドです。

**内容:**

- Docker と docker-compose のセットアップ
- ビルド手順（GCC / MinGW）
- 異なるバックエンドのビルド（SDL2 / GDI / Server）
- カスタマイズとトラブルシューティング
- CI/CD 統合

**対象読者:** ホスト環境を汚さずにビルドしたい開発者、CI/CD 環境を構築したい開発者

**利点:**

- ✅ 依存関係の自動インストール
- ✅ クリーンな環境でのビルド
- ✅ クロスプラットフォーム対応
- ✅ CI/CD との統合が容易

---

### 🏗️ アーキテクチャ（architecture/）

#### [TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md)

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

#### [FEATURES.md](architecture/FEATURES.md) ⚙️

**主要機能リファレンス**

Simutrans の個々の主要機能について、実装の詳細と設計思想を詳しく解説しています。

**内容:**

**🚉 輸送システム**

- **停留所（Halt）システム**: 貨物管理、経路探索、統計、予約システム
- **路線（Line）管理**: スケジュール共有、財務統計、状態管理
- **編成（Convoi）システム**: 物理シミュレーション、状態機械、加速計算
- **スケジュール管理**: 待機条件、循環構造、動的更新

**🏭 経済システム**

- **工場（Factory）システム**: JIT2 生産方式、ブーストシステム、サプライチェーン
- **貨物（Ware）流通**: トランスファー、経路決定、旅客満足度
- **プレイヤー財務**: 収支管理、資産評価、倒産システム

**🌍 世界システム**

- **都市（Stadt）システム**: 成長メカニズム、建物配置、需要生成

**対象読者:**

- ゲームシステムの詳細を理解したい開発者
- 機能追加や改善を検討している方
- AI 開発やバランス調整に取り組む方

**主な図解:**

- 📊 各システムのアーキテクチャ図
- 🔄 処理フローチャート
- 📈 統計・財務の計算フロー
- 🏗️ システム間の連携図

### 🛠️ ツール・ユーティリティ

#### [MAKEOBJ.md](systems/MAKEOBJ.md) 📦

**Pakset 作成ツール完全ガイド**

Simutrans の Pakset（グラフィックス定義ファイル）を作成するための `makeobj` ツールの包括的なドキュメントです。

**内容:**

- makeobj コマンドの全種類（PAK、LIST、DUMP、MERGE、EXTRACT、EXPAND）
- DAT ファイル形式の詳細仕様
- 画像処理と最適化
- デバッグとトラブルシューティング

**対象読者:**

- 新しい Pakset を作成したい方
- Pakset のカスタマイズに取り組む方
- グラフィックスアセットの管理方法を学びたい方

---

#### [NETTOOL.md](systems/NETTOOL.md) 🛠️

**Simutrans ネットワークツール**

Simutrans 専用のネットワークサーバー制御ツール `nettool` の包括的なドキュメントです。

**内容:**

- nettool の概要と TCP/IP 通信の仕組み
- 15 個の全コマンド詳細解説（`send`、`kick`、`ban`、`clients`、`save` など）
- サーバー接続〜切断のシーケンス図
- コマンド実行フロー
- バックアップ・保存の処理手順

**対象読者:**

- Simutrans マルチプレイサーバーを管理する方
- ネットワーク機能の実装を理解したい開発者

---

### 🎮 ゲームシステム

#### [ACHIEVEMENTS.md](systems/ACHIEVEMENTS.md) 🏆

**実績システム**

Simutrans の実績（アチーブメント）システムの完全なドキュメントです。

**内容:**

- 実績システムのアーキテクチャ
- X マクロパターンを使用した実績の定義
- Steam との統合
- 実績の種類と実装例
- Squirrel スクリプトからのアクセス

**対象読者:**

- 新しい実績を追加したい開発者
- Steam 連携機能を理解したい方

---

#### [PLAYER_MANAGEMENT.md](systems/PLAYER_MANAGEMENT.md) 👥

**プレイヤー管理システム**

複数のプレイヤー（人間・AI）の管理、財務、権限設定に関するシステムの詳細解説です。

**内容:**

- プレイヤータイプと権限管理
- 財務システム（資金、負債、倒産）
- 本社（本部）システム
- AI プレイヤーの設定
- セーブ・ロード機構

**対象読者:**

- マルチプレイヤー機能を開発したい方
- 経済システムを理解したい方
- AI システムをカスタマイズしたい方

---

#### [SCENARIO.md](systems/SCENARIO.md) 🎯

**シナリオ・ミッションシステム**

Simutrans のシナリオ（ミッション）システムの完全なドキュメントです。

**内容:**

- シナリオの構造と仕様
- Squirrel スクリプトでのシナリオ実装
- UI タブと目標設定
- 完了追跡とクリア判定
- 実装例とベストプラクティス

**対象読者:**

- シナリオを作成したい コンテンツクリエーター
- スクリプト言語（Squirrel）を学びたい方

---

#### [STEAM_WORKSHOP.md](systems/STEAM_WORKSHOP.md) 🚀

**Steam Workshop 統合**

Steam Workshop を通じたコンテンツ配布・管理機能の詳細解説です。

**内容:**

- Workshop アイテムの種類とカテゴリ
- インストール・アンインストールプロセス
- Steam API との連携
- Pakset の検出と読み込み
- アップロードと更新の方法
- 依存関係の管理

**対象読者:**

- Steam でコンテンツを公開したい方
- Workshop 機能を実装したい開発者
- コンテンツ配布のベストプラクティスを学びたい方

---

### 🔬 詳細な仕様

#### [VEHICLE_TERMINAL_SPEED.md](systems/VEHICLE_TERMINAL_SPEED.md) 🚄

**車両終端速度計算ドキュメント**

車両の重量・積載量に応じた最高速度（終端速度）の計算方法を詳しく解説しています。

**内容:**

- 物理シミュレーションモデル（力学方程式）
- 質量・抵抗・動力の計算式
- 加速と減速のメカニズム
  **対象読者:**

- 車両の挙動を理解したい開発者
- 物理シミュレーションに興味がある方
- バランス調整やゲームデザインに取り組む方

**主な図解:**

- 力と加速度の関係式
- 速度曲線グラフ
- 計算フローチャート

---

#### [VEHICLE_ROUTE_SEARCH.md](systems/VEHICLE_ROUTE_SEARCH.md) 🗺️

**車両経路探索アルゴリズム**

各車両タイプ（道路、鉄道、航空、船舶）がどのように目的地への経路を探索するかを詳しく解説しています。

**内容:**

- 5 種類の車両タイプ別経路探索ロジック
  - 道路車両（road_vehicle）
  - 鉄道車両（rail_vehicle）
  - 航空機（air_vehicle）- 3 段階探索
  - 船舶（water_vehicle）
  - モノレール（monorail_vehicle）
- A\* アルゴリズムの実装詳細
- ヒューリスティック評価関数
- 最適経路の決定方法

**対象読者:**

- 経路探索アルゴリズムを学びたい方
- AI・自動運転に興味がある開発者
- ゲームロジックを深く理解したい方

**主な図解:**

- 各車両タイプの探索フロー（Mermaid フローチャート）
- 航空機の 3 段階探索（離陸 → 巡航 → 着陸）
- A\* 探索の状態遷移図

---

## 📖 ドキュメント活用ガイド

### レベル別おすすめルート

#### 初心者 🌱

1. [PROJECT_OVERVIEW.md](guides/PROJECT_OVERVIEW.md) - プロジェクト全体を把握
2. [DEVELOPMENT_SETUP.md](guides/DEVELOPMENT_SETUP.md) - 環境構築
3. [TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md) - アーキテクチャ理解
4. [FEATURES.md](architecture/FEATURES.md) - 主要機能の詳細

#### 中級者 🚀

1. [FEATURES.md](architecture/FEATURES.md) - システム設計を深掘り
2. [VEHICLE_ROUTE_SEARCH.md](systems/VEHICLE_ROUTE_SEARCH.md) - 経路探索の実装
3. [VEHICLE_TERMINAL_SPEED.md](systems/VEHICLE_TERMINAL_SPEED.md) - 物理シミュレーション

#### 上級者・コントリビューター 🔧

1. [TECHNICAL_ARCHITECTURE.md](architecture/TECHNICAL_ARCHITECTURE.md) - 設計パターン確認
2. [systems/](systems/) - 関心のあるシステムの詳細
3. ソースコードと併せて読む

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
