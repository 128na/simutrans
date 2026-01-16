# Simutrans 技術アーキテクチャ

## システムアーキテクチャ概要

Simutrans は、階層的なオブジェクト指向設計を採用した交通シミュレーションエンジンです。

## 目的

このドキュメントは以下の目的で作成されています:

- **アーキテクチャ理解**: Simutrans の内部構造と設計思想を把握
- **コンポーネント把握**: 主要なクラスとその責任範囲を理解
- **開発指針**: 新機能実装時のガイドライン
- **パフォーマンス最適化**: 効率的なコードを書くためのヒント

**対象読者:**

- コードベースを理解したい開発者
- 機能を実装する開発者
- システム設計を学びたい方

## コアコンポーネント

### 1. ワールド管理 (World Management)

#### karte_t (Map/World)

- **場所**: `src/simutrans/world/`
- **役割**: ゲーム世界全体の管理
- **責任**:
  - planquadrat_t の 2D 配列を保持
  - グローバルなゲーム状態管理
  - 時間管理とシミュレーションループ

#### planquadrat_t (2D Coordinate)

- **役割**: 地図上の単一座標を表現
- **内容**: grund_t の配列（高度レベルごと）

#### grund_t (Ground/Foundation)

- **場所**: `src/simutrans/ground/`
- **役割**: 実際の地形や構造物
- **種類**:
  - 通常の地面
  - トンネル
  - 橋
  - 高速道路
  - 水域

### 2. オブジェクトシステム (Object System)

#### obj_t (Base Object)

- **場所**: `src/simutrans/obj/`
- **役割**: すべてのゲームオブジェクトの基底クラス
- **派生クラス**:
  - 移動オブジェクト（車両、列車、飛行機）
  - 静的オブジェクト（建物、木、標識）
  - インフラ（道路、線路、駅）

#### オブジェクトライフサイクル

```cpp
// 3種類の更新メソッド
class obj_t {
    // 毎フレーム呼ばれる（高速、排他的マップアクセス）
    virtual void sync_step(uint32 delta_t);

    // 低速な処理（ルート検索など、マップ変更可能）
    virtual void step(uint32 delta_t);

    // 季節変化（主に視覚的変更）
    virtual void check_season();
};
```

### 3. 車両システム (Vehicle System)

#### 構造

- **場所**: `src/simutrans/vehicle/`
- **コンポーネント**:
  - 車両定義（vehicle_t）
  - 編成管理（convoi）
  - ルート検索
  - 物理シミュレーション

#### simconvoi.cc/h

- **役割**: 編成（複数車両のグループ）管理
- **機能**:
  - 車両の連結
  - スケジュール管理
  - 経路追跡

### 4. 輸送システム (Transportation System)

#### simhalt.cc/h (Halt/Station)

- **役割**: 駅・停留所管理
- **機能**:
  - 旅客・貨物の待機
  - 乗り換え処理
  - 需要計算

#### simline.cc/h (Line)

- **役割**: 路線管理
- **機能**:
  - 停車駅リスト
  - ダイヤ管理
  - 複数編成の統合管理

### 5. 経済システム (Economy System)

#### simfab.cc/h (Factory)

- **場所**: `src/simutrans/`
- **役割**: 産業施設管理
- **機能**:
  - 原材料の消費
  - 製品の生産
  - 供給チェーン

#### simware.cc/h (Goods/Cargo)

- **役割**: 貨物・旅客データ管理
- **属性**:
  - 種類
  - 量
  - 目的地
  - 経過時間

### 6. プレイヤーシステム (Player System)

- **場所**: `src/simutrans/player/`
- **機能**:
  - 財務管理
  - 権限管理
  - AI 制御

## グラフィックスシステム

### 表示アーキテクチャ

```
Display System
├── simgraph.h          # グラフィック抽象レイヤー
├── display/            # 表示システム
│   ├── simgraph16.cc   # 16bit描画（SDL2）
│   └── simimg.h        # 画像管理
└── sys/                # バックエンド実装
    ├── simsys_s2.cc    # SDL2実装
    └── simsys_w.cc     # GDI実装（Windows）
```

### バックエンド

| バックエンド | ファイル           | プラットフォーム       |
| ------------ | ------------------ | ---------------------- |
| SDL2         | `sys/simsys_s2.cc` | クロスプラットフォーム |
| GDI          | `sys/simsys_w.cc`  | Windows 専用           |
| Server       | -                  | GUI 無し               |

## UI システム

### アーキテクチャ原則

- メインロジックから完全分離
- コンポーネントベース設計
- イベント駆動型

### クラス階層

```
gui_frame_t (基底クラス)
├── gui_komponente_t (コンポーネント基底)
│   ├── button_t (ボタン)
│   ├── scrollbar_t (スクロールバー)
│   ├── textinput_t (テキスト入力)
│   ├── label_t (ラベル)
│   └── ... (その他のコンポーネント)
└── 各種ダイアログクラス
```

### UI コンポーネント

- **場所**: `src/simutrans/gui/components/`
- **使い方**:

  ```cpp
  class my_frame_t : public gui_frame_t {
      button_t my_button;

      my_frame_t() {
          my_button.init(...);
          my_button.add_action_listener(this);
          add_component(&my_button);
      }

      bool action_triggered(gui_action_creator_t*, value_t) override {
          // ボタンクリック処理
      }
  };
  ```

### テーマシステム

- **場所**: `themes.src/`
- **利用可能なテーマ**:
  - standard
  - modern
  - flat
  - aero
  - highcontrast
  - pak64german
  - pak192comic

## ネットワークシステム

### アーキテクチャ

- **場所**: `src/simutrans/network/`
- **機能**:
  - クライアント・サーバーモデル
  - 同期処理
  - チート検出

### コンポーネント

- マルチプレイヤー同期
- パケット管理
- ネットワークコマンド処理

## スクリプトシステム

### Squirrel 統合

- **エンジン**: `src/squirrel/`
- **AI スクリプト**: `simutrans/ai/`
- **API**: `src/simutrans/script/`

### 機能

- AI 制御
- シナリオスクリプティング
- カスタムロジック

## サウンドシステム

### 構造

```
Sound System
├── simsound.cc/h       # サウンド抽象レイヤー
└── sound/              # バックエンド実装
    ├── sdl2_sound.cc   # SDL2実装
    └── sdl_sound.cc    # SDL1実装（レガシー）
```

### MIDI 再生

- **FluidSynth**: Linux/Mac 推奨
- **SDL2_mixer**: 代替実装

## データ管理

### dataobj/ (Data Objects)

- **translator**: 多言語対応
- **loadsave**: セーブ/ロード処理
- **environment**: 環境設定
- **schedule**: スケジュール管理

### descriptor/ (Descriptors)

オブジェクトの静的定義：

- 車両仕様
- 建物定義
- 地形タイプ
- 商品タイプ

## I/O システム

### io/ ディレクトリ

- ファイル読み込み
- セーブファイル管理
- Pakset 読み込み

### サポートフォーマット

- **.sve**: Simutrans セーブファイル
- **.pak**: Pakset アーカイブ
- 圧縮: zlib, bzip2, zstd

## ビルダーシステム

### builder/ ディレクトリ

- **役割**: ゲーム内建設ツールのロジック
- **機能**:
  - 道路・線路建設
  - 建物配置
  - 地形変更

## テンプレートライブラリ (TPL)

### tpl/ ディレクトリ

カスタムコンテナ実装：

- `vector_tpl`: 動的配列
- `slist_tpl`: 単方向リスト
- `hashtable_tpl`: ハッシュテーブル
- `array_tpl`: 固定サイズ配列
- `quickstone_tpl`: 高速参照

### 使用理由

- STL に依存しない
- ゲーム特化の最適化
- メモリ管理の明示的制御

## ユーティリティ

### utils/ ディレクトリ

- **cbuffer**: 文字バッファ
- **sha1**: ハッシュ計算
- **searchfolder**: ファイル検索
- **simstring**: 文字列処理

## メモリ管理

### simmem.cc/h

- カスタムメモリアロケータ
- メモリプール管理
- デバッグサポート

## デバッグシステム

### simdebug.cc/h

- ログシステム
- アサーション
- デバッグ出力

### 使い方

```cpp
dbg->message("tag", "Message %d", value);
dbg->warning("tag", "Warning!");
dbg->error("tag", "Error!");
```

## プラットフォーム固有コード

### android/

- Android アプリケーション統合
- JNI インターフェース

### OSX/

- macOS バンドル
- ネイティブメニュー

### Windows/

- リソース管理
- Windows API 統合

### linux/

- Linux 固有の処理

## ツールシステム

### tool/ ディレクトリ

ゲーム内ツール実装：

- 建設ツール
- 削除ツール
- 調査ツール
- 管理ツール

## パフォーマンス最適化

### 設計原則

1. **キャッシング**: 頻繁に使用されるデータをキャッシュ
2. **遅延評価**: 必要になるまで計算を遅延
3. **空間分割**: マップをセクターに分割
4. **イベント駆動**: ポーリングではなくイベント

### 更新戦略

- **sync_step()**: 重要な同期処理のみ
- **step()**: 低優先度の処理
- **check_season()**: 視覚的更新のみ

## ビルド設定

### コンパイルオプション

CMake で制御可能：

- `SIMUTRANS_BACKEND`: グラフィックバックエンド選択
- `SIMUTRANS_USE_REVISION`: バージョン番号指定
- `CMAKE_BUILD_TYPE`: ビルドタイプ（Debug/Release）

### プリプロセッサマクロ

- `MULTI_THREAD`: マルチスレッド対応
- `USE_SOFTPOINTER`: ソフトウェアポインタ
- `COLOUR_DEPTH`: 色深度設定

## セキュリティ

### ネットワークセキュリティ

- パケット検証
- チート検出
- 同期チェック

### セーブファイル整合性

- チェックサム検証
- バージョン互換性チェック

## 拡張性

### モジュール性

各システムは比較的独立：

- 表示システム
- サウンドシステム
- ネットワークシステム
- スクリプトシステム

### プラグインサポート

- Pakset（データモジュール）
- AI スクリプト
- シナリオ

---

## 関連ファイル

### コアシステム

- **ワールド管理**: `src/simutrans/world/`, `src/simutrans/simworld.{h,cc}`
- **オブジェクトシステム**: `src/simutrans/obj/`
- **車両システム**: `src/simutrans/vehicle/`

### UI/グラフィックス

- **描画システム**: `src/simutrans/display/`
- **GUI**: `src/simutrans/gui/`

### データ管理

- **セーブ/ロード**: `src/simutrans/dataobj/loadsave.{h,cc}`
- **ネットワーク**: `src/simutrans/network/`

### ドキュメント

- **コーディング規約**: `documentation/coding_styles.txt`
- **コードについて**: `documentation/about-the-code.txt`

---

## まとめ

Simutrans の技術アーキテクチャは、長年の開発で洗練されてきました:

**主な特徴**:

- **階層的設計**: ワールド → グラウンド → オブジェクトの明確な階層
- **モジュラー構造**: 各コンポーネントが独立して動作
- **プラグインシステム**: Pakset でコンテンツを動的に追加
- **パフォーマンス最適化**: メモリ効率、キャッシュフレンドリー、マルチスレッド対応
- **クロスプラットフォーム**: ポータブルな C++ コードで複数 OS に対応

このアーキテクチャを理解したら、次は [FEATURES.md](FEATURES.md) で具体的なシステムを深掘りしましょう。

---

## 今後の開発

### 最新情報

- [International Forum](https://forum.simutrans.com/)
- [Technical Documentation](https://forum.simutrans.com/index.php/board,112.0.html)

### 開発ガイドライン

- `documentation/coding_styles.txt`
- `documentation/about-the-code.txt`
