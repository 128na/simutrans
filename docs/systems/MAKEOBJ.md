# Makeobj - Simutrans Pakset 作成ツール

## 概要

**makeobj** は、Simutrans のグラフィックアセットとゲームオブジェクトを定義した `.dat` ファイルを、ゲームで読み込める `.pak` ファイルに変換するコマンドラインツールです。Pakset 制作者や MOD 開発者が使用します。

## 目的

makeobj は、以下のような用途で使用されます：

- **Pakset 作成**: `.dat` ファイルと `.png` 画像から `.pak` ファイルを生成
- **Pak 管理**: 複数の pak ファイルの結合、分解、内容確認
- **デバッグ**: Pak ファイルの構造やチェックサムの検証
- **Dat ファイル展開**: 最小化記法を完全記法に展開

## インストールと起動

### ビルド

Simutrans のビルドシステムの一部として自動的にビルドされます：

```bash
# CMake を使用する場合
cmake -B build
cmake --build build --target makeobj

# Make を使用する場合
make makeobj
```

ビルド成功後、実行ファイルは以下の場所に配置されます：

- Linux/macOS: `build/makeobj`
- Windows: `build/makeobj.exe`

## 基本的な使用方法

### 一般的なコマンド形式

```bash
makeobj [QUIET|VERBOSE|DEBUG] <コマンド> <パラメータ>
```

### デバッグレベル

| オプション | 説明                                                 |
| ---------- | ---------------------------------------------------- |
| `QUIET`    | エラーのみ表示（著作権メッセージも非表示）           |
| `VERBOSE`  | メッセージとエラーを表示（未使用行、未割当エントリ） |
| `DEBUG`    | 詳細なデバッグ情報を出力                             |

## 利用可能なコマンド

### 1. `CAPABILITIES`

**説明**: makeobj がサポートするオブジェクトタイプの一覧を表示します。

**使用例**:

```bash
makeobj CAPABILITIES
```

**出力例**:

```
Makeobj version ... for Simutrans ...
Supported object types:
  - vehicle (車両)
  - building (建物)
  - way (道路・線路)
  - bridge (橋)
  - tunnel (トンネル)
  - tree (木)
  - good (商品)
  - factory (工場)
  ...
```

---

### 2. `PAK` - Pak ファイル作成

**説明**: `.dat` ファイルから `.pak` ファイルを作成します（最も一般的な使用方法）。

**構文**:

```bash
makeobj PAK <出力先> <入力ファイル/ディレクトリ>
```

**使用例**:

```bash
# 単一の dat ファイルから pak を作成
makeobj PAK myvehicle.pak vehicles/bus.dat

# ディレクトリ内のすべての dat ファイルから pak を作成
makeobj PAK output/vehicles.pak vehicles/

# 複数のファイルを指定
makeobj PAK buildings.pak building1.dat building2.dat building3.dat

# デフォルト（カレントディレクトリ）
makeobj PAK
```

**注意事項**:

- 画像ファイル（`.png`）は `.dat` ファイルで参照されるため、適切なパスに配置する必要があります
- 出力先がディレクトリで終わる場合（`/` または `\`）、ディレクトリ内に個別の pak ファイルが作成されます

---

### 3. `PAK128` / `PAK64` / `PAK256` 等 - カスタムタイルサイズ

**説明**: 特定のタイルサイズ用の pak ファイルを作成します。

**構文**:

```bash
makeobj PAK<サイズ> <出力先> <入力ファイル>
```

**使用例**:

```bash
# 128x128 タイルサイズ用
makeobj PAK128 output.pak vehicles/

# 64x64 タイルサイズ用
makeobj PAK64 output.pak buildings/

# 16 から 32767 までのサイズをサポート（ただし 255 までがテスト済み）
makeobj PAK192 output.pak pak192.comic/
```

---

### 4. `LIST` - Pak ファイル内容の一覧表示

**説明**: Pak ファイルに含まれるオブジェクトの一覧を表示します。

**構文**:

```bash
makeobj LIST <pak ファイル> [<pak ファイル2> ...]
```

**使用例**:

```bash
# 単一ファイルの内容を表示
makeobj LIST vehicles.pak

# 複数ファイルを一度に表示
makeobj LIST vehicles.pak buildings.pak
```

**出力例**:

```
vehicles.pak:
  - vehicle: bus_mercedes_o305
  - vehicle: train_ice3
  - vehicle: airplane_boeing747
...
```

---

### 5. `DUMP` - Pak ファイル内部構造の表示

**説明**: Pak ファイルの内部ノード構造を詳細に表示します（デバッグ用）。

**構文**:

```bash
makeobj DUMP <出力先> <pak ファイル> [<pak ファイル2> ...]
```

**使用例**:

```bash
makeobj DUMP debug_output.txt vehicles.pak
```

**出力内容**:

- ノードタイプ
- データサイズ
- 子ノード情報
- チェックサム

---

### 6. `MERGE` - Pak ファイルの結合

**説明**: 複数の pak ファイルを 1 つの pak ライブラリにマージします。

**構文**:

```bash
makeobj MERGE <出力 pak ファイル> <入力 pak ファイル1> [<pak ファイル2> ...]
```

**使用例**:

```bash
# 複数の pak ファイルを 1 つに結合
makeobj MERGE combined.pak vehicles.pak buildings.pak bridges.pak

# ディレクトリ内のすべての pak をマージ
makeobj MERGE all.pak pakfiles/
```

**用途**:

- 配布用に複数の小さな pak を統合
- Addon の管理を簡略化

---

### 7. `EXTRACT` - Pak ファイルの分解

**説明**: Pak アーカイブから個別の pak ファイルを抽出します。

**構文**:

```bash
makeobj EXTRACT <pak アーカイブ>
```

**使用例**:

```bash
makeobj EXTRACT combined.pak
```

**動作**:

- アーカイブ内の各オブジェクトを個別の `.pak` ファイルとして抽出
- 元のオブジェクト名がファイル名になります

---

### 8. `EXPAND` - Dat ファイルの展開

**説明**: 最小化記法で書かれた `.dat` ファイルを完全記法に展開します。

**構文**:

```bash
makeobj EXPAND <出力先> <dat ファイル> [<dat ファイル2> ...]
```

**使用例**:

```bash
makeobj EXPAND expanded/ vehicles/*.dat
```

**用途**:

- 最小化記法のドキュメント化
- 初心者向けの学習資料作成
- デバッグ時の完全な設定確認

---

## Dat ファイルの基本構造

`.dat` ファイルは、Simutrans のオブジェクトを定義するテキストファイルです。

### 基本的な書式

```dat
Obj=vehicle
Name=bus_example
copyright=YourName
waytype=road
engine_type=diesel
speed=80
power=150
capacity=50
weight=12
cost=50000
runningcost=80
intro_year=1990
intro_month=1
retire_year=2020
retire_month=12

# 画像の指定
EmptyImage[S]=buses.0.0
EmptyImage[E]=buses.0.1
EmptyImage[N]=buses.0.2
EmptyImage[W]=buses.0.3
```

### 主要なオブジェクトタイプ

| オブジェクトタイプ | 説明             | 主要パラメータ                        |
| ------------------ | ---------------- | ------------------------------------- |
| `vehicle`          | 車両（陸海空）   | speed, power, capacity, waytype       |
| `building`         | 建物             | type, level, dims, passengers         |
| `way`              | 道路・線路       | waytype, speed, cost, maintenance     |
| `bridge`           | 橋               | waytype, max_length, max_weight       |
| `tunnel`           | トンネル         | waytype, cost, maintenance            |
| `good`             | 商品             | value, weight, color                  |
| `factory`          | 工場             | productivity, range, inputs, outputs  |
| `tree`             | 木               | distribution_weight, climate          |
| `citycar`          | 市民の自動車     | speed, weight                         |
| `pedestrian`       | 歩行者           | distribution_weight                   |
| `roadsign`         | 道路標識         | waytype, cost, single_way, free_route |
| `citybuilding`     | 都市の建物       | level, passengers, mail               |
| `attraction`       | 観光地           | level, passengers, maintenance        |
| `groundobj`        | 地面オブジェクト | climates, distribution_weight         |
| `smoke`            | 煙エフェクト     | smoke_time                            |
| `field`            | 農地             | production, capacity                  |
| `crossing`         | 踏切             | waytype, cost                         |
| `groundobj`        | 装飾オブジェクト | distribution_weight                   |

---

## 画像ファイルの扱い

### 画像フォーマット

- **対応フォーマット**: PNG（推奨）、BMP
- **透過**: PNG のアルファチャンネルをサポート
- **特殊色**:
  - `#FF00FF` (マゼンタ): 透明色として扱われる（レガシー）
  - プレイヤーカラー: 特定の色範囲が動的に置き換えられる

### 画像の指定方法

```dat
# 方向別の画像
EmptyImage[S]=filename.0.0
EmptyImage[E]=filename.0.1
EmptyImage[N]=filename.0.2
EmptyImage[W]=filename.0.3

# 画像のオフセット
EmptyImage[S]=filename.0.0,xoff,yoff

# 複数フレーム（アニメーション）
Image[0][0]=filename.0.0
Image[0][1]=filename.0.1
Image[0][2]=filename.0.2
```

### 画像ファイルの配置

画像ファイルは以下の場所から検索されます：

1. `.dat` ファイルと同じディレクトリ
2. 相対パスで指定されたディレクトリ
3. makeobj 実行時のカレントディレクトリ

---

## よくあるエラーと対処法

### 1. `Could not open file: <filename>.png`

**原因**: 画像ファイルが見つからない

**対処法**:

- 画像ファイルのパスを確認
- ファイル名の大文字小文字を確認（Linux/macOS では区別されます）
- `.dat` ファイル内のパス指定を確認

### 2. `WARNING: Duplicate object`

**原因**: 同じ名前のオブジェクトが複数定義されている

**対処法**:

- `Name=` パラメータが一意であることを確認
- 意図的な上書きの場合は警告を無視しても可

### 3. `Unknown parameter: <param>`

**原因**: サポートされていないパラメータが使用されている

**対処法**:

- パラメータ名のスペルミスを確認
- 古い makeobj バージョンでは新しいパラメータが未サポートの可能性
- `makeobj CAPABILITIES` でサポート内容を確認

### 4. `Error reading image`

**原因**: 画像ファイルが破損しているか、未対応のフォーマット

**対処法**:

- PNG ファイルとして正しく保存されているか確認
- 画像編集ソフトで再保存を試す
- 色深度を確認（24bit RGB または 32bit RGBA 推奨）

---

## DEBUG モードの活用

`DEBUG` オプションを使用すると、詳細な処理情報が出力されます：

```bash
makeobj DEBUG PAK output.pak input/
```

**出力される情報**:

```
Source:  obj=vehicle
Source:  Name=bus_example
Image:   buses.png
X:       0
Y:       0
Off X:   -20
Off Y:   -30
Width:   64
Height:  32
...
```

**活用例**:

- 画像のオフセットが正しく適用されているか確認
- どの `.dat` ファイルが読み込まれたか追跡
- パラメータの解釈を確認

---

## 高度な使用例

### 1. バッチ処理（Windows）

```bat
@echo off
for %%f in (vehicles\*.dat) do (
    makeobj PAK pak\vehicles\ vehicles\%%f
)
```

### 2. バッチ処理（Linux/macOS）

```bash
#!/bin/bash
for datfile in vehicles/*.dat; do
    basename=$(basename "$datfile" .dat)
    makeobj PAK "pak/vehicles/$basename.pak" "$datfile"
done
```

### 3. Makefile を使った自動化

```makefile
SOURCES := $(wildcard *.dat)
TARGETS := $(SOURCES:.dat=.pak)

all: $(TARGETS)

%.pak: %.dat
	makeobj PAK $@ $<

clean:
	rm -f *.pak
```

---

## Pakset 開発のワークフロー

### 基本的な手順

1. **画像の準備**

   - PNG 形式で画像を作成
   - 透過処理を適用
   - 適切なサイズとオフセットを設定

2. **Dat ファイルの作成**

   - オブジェクトタイプの選択
   - パラメータの設定
   - 画像の参照を追加

3. **Pak ファイルの生成**

   ```bash
   makeobj PAK output.pak myobject.dat
   ```

4. **テスト**

   - 生成された pak ファイルを Simutrans のデータディレクトリにコピー
   - ゲームを起動して動作確認

5. **デバッグ**
   - エラーがあれば DEBUG モードで詳細を確認
   - `LIST` コマンドで pak の内容を検証

### ディレクトリ構造の例

```
my-pakset/
├── vehicles/
│   ├── buses.dat
│   ├── buses.png
│   ├── trains.dat
│   └── trains.png
├── buildings/
│   ├── residential.dat
│   ├── residential.png
│   ├── commercial.dat
│   └── commercial.png
├── output/
│   ├── vehicles.pak
│   └── buildings.pak
└── Makefile
```

---

## 関連ファイルとディレクトリ

- **ソースコード**: `src/makeobj/`
- **ビルド設定**: `src/makeobj/CMakeLists.txt`, `src/makeobj/Makefile`
- **画像変換システム**: [IMAGE_CONVERSION.md](IMAGE_CONVERSION.md) - PNG ⇔ Binary 変換の詳細
- **Dat ファイルサンプル**: 各 Pakset のソースリポジトリ
- **公式 Pakset**:
  - pak64: https://github.com/simutrans/simutrans/tree/master/paksets
  - pak128: https://github.com/pak128/

---

## 参考リンク

- [Simutrans 公式サイト](https://www.simutrans.com/)
- [Pakset 作成ガイド（英語）](https://simutrans-germany.com/wiki/wiki/en_MakeObj)
- [Dat ファイルリファレンス](https://simutrans-germany.com/wiki/wiki/en_Dat_file)
- [公式フォーラム - Pakset 開発](https://forum.simutrans.com/index.php/board,34.0.html)

---

## トラブルシューティング

### makeobj が見つからない

```bash
# ビルドディレクトリを確認
ls build/makeobj*

# PATH に追加
export PATH=$PATH:/path/to/simutrans/build
```

### メモリ不足エラー

大量のオブジェクトを一度に処理する場合、メモリ不足になることがあります。

**対処法**:

- 複数の小さな pak ファイルに分割
- 後で `MERGE` コマンドで結合

### 文字化け

**原因**: Dat ファイルのエンコーディングが UTF-8 でない

**対処法**:

- エディタで UTF-8 として保存
- BOM なし UTF-8 を推奨

---

## まとめ

makeobj は Simutrans の Pakset 制作に不可欠なツールです。基本的な使い方から高度なバッチ処理まで、この文書で解説した内容を活用して、効率的に Pakset を開発できます。

疑問点や問題がある場合は、公式フォーラムやコミュニティで質問してください。
