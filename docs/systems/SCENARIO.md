# Simutrans シナリオシステム

## 概要

Simutrans の**シナリオシステム**は、特定の目標や条件を持ったゲームモードを提供する機能です。通常のフリープレイとは異なり、明確なゲーム目標が設定され、達成率を追跡できます。シナリオは Squirrel スクリプトで記述され、柔軟なゲームプレイ体験を実現します。

## 目的

シナリオシステムは、以下のような用途で使用されます：

- **チュートリアル**: 初心者向けの学習シナリオ
- **チャレンジ**: 経験者向けの高難易度ミッション
- **ストーリー**: 特定のテーマや歴史に基づいた物語性のあるゲーム
- **制約プレイ**: 特定のルールや制限の下でのプレイ
- **コンテスト**: コミュニティ内での競争

## システムアーキテクチャ

### コンポーネント

```
Squirrel スクリプト (.nut)
    ↓
scenario_t (シナリオ管理)
    ↓
scenario_info_t (UI 表示)
    ↓
player_t (進捗追跡)
```

**関連ファイル**:

- `src/simutrans/dataobj/scenario.{cc,h}` - シナリオロジック
- `src/simutrans/gui/scenario_info.{cc,h}` - シナリオ情報ウィンドウ
- `src/simutrans/player/simplay.{cc,h}` - プレイヤーごとの進捗
- `simutrans/script/*.nut` - サンプルシナリオスクリプト

---

## シナリオの構成要素

### 1. セーブファイル

シナリオは既存のセーブファイル（`.sve`）をベースにします：

- 初期のマップ状態
- 配置された都市、工場、道路
- プレイヤーの初期資金
- 開始時刻（年月）

### 2. Squirrel スクリプト

シナリオのロジックと目標を定義するスクリプトファイル（`.nut`）：

- 目標の定義
- ルールの設定
- 進捗の計算
- 情報テキストの提供

---

## シナリオスクリプトの基本構造

### テンプレートファイル

`simutrans/script/new_scenario_template.nut` が基本テンプレートです：

```squirrel
/// ロードするセーブファイルを指定
map.file = "savegame.sve"

/// 短い説明文（財務ウィンドウに表示）
scenario.short_description = "Template Scenario"

/// 作者情報
scenario.author = "User"
scenario.version = "0.0"
scenario.translation = "The Unknown Translators"

/// 必要な API バージョン
scenario.api = "120.0"

/**
 * プレイヤーごとの情報を返す関数
 * pl == 0: 最初のプレイヤースロット
 * pl == 1: Public player
 */
function get_info_text(pl) {
    // 一般情報
    return "シナリオの概要をここに記載"
}

function get_rule_text(pl) {
    // 遵守すべきルール
    return "このシナリオのルール"
}

function get_result_text(pl) {
    // 現在の進捗状況
    return "達成率: " + get_completion() + "%"
}

function get_goal_text(pl) {
    // 目標の説明
    return "このシナリオの目標"
}

function get_about_text(pl) {
    // シナリオについての情報
    return "作者: " + scenario.author
}

function get_error_text(pl) {
    // エラーメッセージ（デバッグ用）
    return ""
}

function get_debug_text(pl) {
    // デバッグ情報
    return ""
}
```

---

## シナリオ情報ウィンドウ

### UI コンポーネント

シナリオ情報ウィンドウ（`scenario_info_t`）は 6 つのタブで構成されます：

| タブ   | 関数名                                  | 説明                 |
| ------ | --------------------------------------- | -------------------- |
| Info   | `get_info_text()`                       | シナリオの一般情報   |
| Goal   | `get_goal_text()`                       | 達成すべき目標       |
| Rule   | `get_rule_text()`                       | 遵守すべきルール     |
| Result | `get_result_text()`                     | 現在の進捗状況と結果 |
| About  | `get_about_text()`                      | 作者、バージョン情報 |
| Debug  | `get_debug_text()` + `get_error_text()` | エラーとデバッグ情報 |

### 動的テキストの更新

テキストは動的に更新され、ゲームの進行に応じて変化します：

```cpp
void scenario_info_t::draw(scr_coord pos, scr_size size) {
    // タブごとのテキストを更新
    update_scenario_texts();
    gui_frame_t::draw(pos, size);
}

bool scenario_info_t::update_dynamic_texts(
    gui_flowtext_t *flow,
    dynamic_string *text,
    scr_size size,
    bool init
) {
    // テキストが変更された場合のみ再描画
    if (text->has_changed() || init) {
        flow->set_text(text->get_text());
        return true;
    }
    return false;
}
```

---

## 達成率の計算

### プレイヤーごとの進捗

各プレイヤーは個別に達成率を持ちます：

```cpp
class player_t {
private:
    sint32 scenario_completion;  // 0-100 のパーセンテージ

public:
    sint32 get_scenario_completion() const {
        return scenario_completion;
    }

    void set_scenario_completion(sint32 percent) {
        scenario_completion = clamp(percent, 0, 100);
    }
};
```

### Squirrel スクリプトからの設定

スクリプトから達成率を更新できます：

```squirrel
function step() {
    local completion = calculate_completion()
    world.set_scenario_completion(player, completion)
}

function calculate_completion() {
    // 条件のチェック
    local delivered = get_delivered_goods()
    local required = get_required_goods()

    return (delivered * 100) / required
}
```

---

## シナリオの種類

### 1. 輸送チャレンジ

**目標**: 特定の量の旅客や貨物を輸送する

**例**:

```squirrel
function get_goal_text(pl) {
    return "月間 1000 人の旅客を輸送してください"
}

function step() {
    local passengers = get_monthly_passengers(player)
    local completion = min(100, (passengers * 100) / 1000)
    world.set_scenario_completion(player, completion)
}
```

### 2. 工場供給シナリオ

**目標**: 特定の工場とそのサプライチェーン全体に供給する

**例**:

```squirrel
function get_goal_text(pl) {
    return "鉄鋼工場とすべての供給工場を接続してください"
}

function check_factories() {
    local factories = ["鉄鉱石", "石炭", "鉄鋼"]
    local connected = 0

    foreach(name in factories) {
        if (is_factory_connected(name, player)) {
            connected++
        }
    }

    return (connected * 100) / factories.len()
}
```

### 3. 都市成長シナリオ

**目標**: 特定の都市を一定規模まで成長させる

**例**:

```squirrel
function get_goal_text(pl) {
    return "首都の人口を 10000 人まで増やしてください"
}

function check_city_growth() {
    local city = get_city("Capital")
    local population = city.get_population()

    return min(100, (population * 100) / 10000)
}
```

### 4. 収支目標シナリオ

**目標**: 特定の財務目標を達成する

**例**:

```squirrel
function get_goal_text(pl) {
    return "口座残高を 500 万まで増やしてください"
}

function check_finances() {
    local balance = player.get_account_balance()
    local target = 5000000

    if (balance >= target) {
        return 100
    }
    return max(0, (balance * 100) / target)
}
```

### 5. 時間制限チャレンジ

**目標**: 制限時間内に目標を達成する

**例**:

```squirrel
local deadline_year = 1920
local deadline_month = 12

function get_goal_text(pl) {
    return format("1920年までに目標を達成してください\n残り時間: %d年%d月",
                  get_remaining_years(), get_remaining_months())
}

function is_out_of_time() {
    local current = world.get_time()
    return current.year > deadline_year ||
           (current.year == deadline_year && current.month > deadline_month)
}
```

---

## シナリオ API

### ワールド情報の取得

```squirrel
// 現在の年月を取得
local time = world.get_time()
print(time.year + "-" + time.month)

// プレイヤー情報の取得
local player = world.get_player(0)
local balance = player.get_account_balance()
local name = player.get_name()

// 都市情報の取得
local cities = world.get_city_list()
foreach(city in cities) {
    print(city.get_name() + ": " + city.get_population())
}

// 工場情報の取得
local factories = world.get_factory_list()
foreach(factory in factories) {
    print(factory.get_name())
}
```

### イベントフック

```squirrel
// 毎ステップ呼ばれる（月次）
function step() {
    // ゲームの状態をチェック
    update_completion()
}

// 特定のイベント発生時
function on_monthly_change() {
    // 月が変わった時の処理
}

function on_convoy_arrived(convoy, halt) {
    // 編成が停留所に到着した時
}

function is_scenario_completed(pl) {
    // シナリオが完了したか確認
    return get_completion() >= 100
}
```

### 制約の実装

```squirrel
// ツールの使用を制限
function is_tool_allowed(tool_id, player, pos) {
    // 特定のツールを禁止
    if (tool_id == TOOL_RAISE_LAND) {
        return "このシナリオでは地形変更は禁止されています"
    }
    return null  // 許可
}

// 車両タイプを制限
function is_vehicle_allowed(vehicle_name, player) {
    // 特定の車両のみ許可
    local allowed = ["bus_standard", "train_steam"]

    if (allowed.find(vehicle_name) != null) {
        return null  // 許可
    }
    return "このシナリオでは使用できない車両です"
}
```

---

## シナリオの作成手順

### 1. マップの準備

1. Simutrans でマップを作成
2. 初期状態を設定（都市、工場の配置）
3. セーブファイルとして保存

```
simutrans/scenario/my_scenario/map.sve
```

### 2. スクリプトの作成

1. テンプレートをコピー
2. 目標とルールを定義
3. 達成率の計算ロジックを実装

```
simutrans/scenario/my_scenario/scenario.nut
```

### 3. テキストの記述

各タブの内容を記述：

```squirrel
function get_info_text(pl) {
    return ttext("このシナリオでは...\n\n" +
                 "初心者でも楽しめるように設計されています。")
}

function get_goal_text(pl) {
    return ttext("目標:\n" +
                 "* 3つの都市を接続\n" +
                 "* 月間収益 10000\n" +
                 "* 工場を5つ接続")
}

function get_rule_text(pl) {
    return ttext("ルール:\n" +
                 "* 航空機は使用禁止\n" +
                 "* 地形変更は最小限に")
}
```

### 4. テストとデバッグ

```squirrel
function get_debug_text(pl) {
    return format(
        "Cities: %d\n" +
        "Factories: %d\n" +
        "Revenue: %d\n" +
        "Completion: %d%%",
        get_connected_cities(),
        get_connected_factories(),
        get_monthly_revenue(),
        get_completion()
    )
}
```

---

## 実装例：完全なシナリオ

```squirrel
/// simutrans/scenario/beginner_challenge/scenario.nut

map.file = "beginner_map.sve"

scenario.short_description = "初心者向けチャレンジ"
scenario.author = "Simutrans Team"
scenario.version = "1.0"
scenario.api = "120.0"

// 目標値
local target_cities = 3
local target_revenue = 10000
local deadline_year = 1900

// 進捗状態
local connected_cities = 0
local monthly_revenue = 0

function get_info_text(pl) {
    return ttext("初心者向けの簡単なチャレンジシナリオです。\n\n" +
                 "3つの小さな町を接続して、月間収益を上げましょう。")
}

function get_goal_text(pl) {
    return format(
        "目標:\n" +
        "* %d つの都市を接続する (%d / %d)\n" +
        "* 月間収益 %d を達成する (現在: %d)\n" +
        "* %d 年までに達成する",
        target_cities, connected_cities, target_cities,
        target_revenue, monthly_revenue,
        deadline_year
    )
}

function get_rule_text(pl) {
    return ttext("ルール:\n" +
                 "* バスと列車のみ使用可能\n" +
                 "* 地形変更は禁止\n" +
                 "* 借金は最小限に")
}

function get_result_text(pl) {
    local completion = get_completion()
    local status = ""

    if (completion >= 100) {
        status = "おめでとうございます！シナリオを達成しました！"
    } else if (is_out_of_time()) {
        status = "時間切れです。もう一度挑戦してください。"
    } else {
        status = format("進捗: %d%%", completion)
    }

    return status + "\n\n" + get_goal_text(pl)
}

function get_about_text(pl) {
    return format(
        "シナリオ: %s\n" +
        "作者: %s\n" +
        "バージョン: %s\n" +
        "API: %s",
        scenario.short_description,
        scenario.author,
        scenario.version,
        scenario.api
    )
}

function get_debug_text(pl) {
    local time = world.get_time()
    return format(
        "=== Debug Info ===\n" +
        "Current: %d/%d\n" +
        "Cities: %d / %d\n" +
        "Revenue: %d / %d\n" +
        "Completion: %d%%\n",
        time.year, time.month,
        connected_cities, target_cities,
        monthly_revenue, target_revenue,
        get_completion()
    )
}

// 毎月呼ばれる
function step() {
    update_statistics()
    local completion = calculate_completion()
    world.set_scenario_completion(0, completion)
}

function update_statistics() {
    connected_cities = count_connected_cities()
    monthly_revenue = get_player_monthly_revenue(0)
}

function calculate_completion() {
    local city_completion = (connected_cities * 50) / target_cities
    local revenue_completion = min(50, (monthly_revenue * 50) / target_revenue)

    return city_completion + revenue_completion
}

function count_connected_cities() {
    local count = 0
    local cities = world.get_city_list()

    foreach(city in cities) {
        if (has_player_halt_in_city(city, 0)) {
            count++
        }
    }

    return count
}

function has_player_halt_in_city(city, pl) {
    // 都市内にプレイヤーの停留所があるか確認
    // （実装は省略）
    return false
}

function get_player_monthly_revenue(pl) {
    local player = world.get_player(pl)
    return player.get_finance_history(1, 0)  // 先月の収益
}

function is_out_of_time() {
    local time = world.get_time()
    return time.year > deadline_year
}

// ツール制限
function is_tool_allowed(tool_id, pl, pos) {
    // 地形変更ツールを禁止
    if (tool_id == tool_raise_land || tool_id == tool_lower_land) {
        return "このシナリオでは地形変更は禁止されています"
    }
    return null
}

function is_schedule_allowed(pl, schedule) {
    return null  // すべて許可
}
```

---

## シナリオのパッケージング

### ディレクトリ構造

```
simutrans/scenario/
└── my_scenario/
    ├── scenario.nut      # メインスクリプト
    ├── map.sve           # セーブファイル
    ├── readme.txt        # 説明文（オプション）
    └── preview.png       # プレビュー画像（オプション）
```

### 配布

シナリオを配布する場合：

1. フォルダごと ZIP 圧縮
2. フォーラムや GitHub で公開
3. インストール手順を記載：
   ```
   解凍して simutrans/scenario/ に配置してください
   ```

---

## デバッグのヒント

### ログ出力

```squirrel
function debug_print(message) {
    world.debug_print("[Scenario] " + message)
}

function step() {
    debug_print("Checking completion...")
    local comp = calculate_completion()
    debug_print("Completion: " + comp + "%")
}
```

### エラーハンドリング

```squirrel
function get_error_text(pl) {
    try {
        // 何か処理
        update_statistics()
        return ""
    } catch (e) {
        return "Error: " + e
    }
}
```

### テストモード

```squirrel
// デバッグ用の簡単な達成条件
local debug_mode = true

function calculate_completion() {
    if (debug_mode) {
        return 100  // すぐに完了扱い
    }

    // 通常の計算
    return actual_calculation()
}
```

---

## 既存シナリオの例

### Simutrans 付属シナリオ

| シナリオ名          | 説明                         | 難易度 |
| ------------------- | ---------------------------- | ------ |
| Tutorial            | 基本操作を学ぶチュートリアル | 初級   |
| Connect Cities      | 複数の都市を接続する         | 初級   |
| Supply Factory      | 工場のサプライチェーンを構築 | 中級   |
| Passenger Challenge | 大量の旅客を効率的に輸送     | 中級   |
| Economic Boom       | 経済を成長させる             | 上級   |
| Time Trial          | 時間制限内に目標を達成       | 上級   |

---

## パフォーマンスの考慮事項

### 1. 重い計算を避ける

```squirrel
// 悪い例
function step() {
    // 毎月全工場をチェック（遅い）
    foreach(factory in world.get_factory_list()) {
        check_factory(factory)
    }
}

// 良い例
local check_counter = 0
function step() {
    // 3ヶ月に1回だけチェック
    check_counter++
    if (check_counter >= 3) {
        check_counter = 0
        check_all_factories()
    }
}
```

### 2. キャッシュの活用

```squirrel
local cached_cities = null
local cache_timestamp = 0

function get_cities() {
    local current_time = world.get_time().ticks

    // 1年ごとにキャッシュを更新
    if (cached_cities == null || current_time - cache_timestamp > 12) {
        cached_cities = world.get_city_list()
        cache_timestamp = current_time
    }

    return cached_cities
}
```

---

## 関連ファイル

- **シナリオシステム**: `src/simutrans/dataobj/scenario.{cc,h}`
- **UI**: `src/simutrans/gui/scenario_info.{cc,h}`
- **スクリプトサンプル**: `simutrans/script/new_scenario_template.nut`
- **Squirrel API**: `src/simutrans/script/api/`

---

## 参考リンク

- [Squirrel 言語リファレンス](http://squirrel-lang.org/)
- [Simutrans Scripting API（英語）](https://simutrans-germany.com/wiki/wiki/en_Script)
- [シナリオ作成ガイド（英語）](https://forum.simutrans.com/)

---

## まとめ

Simutrans のシナリオシステムは、Squirrel スクリプトによる柔軟な目標設定と進捗追跡を提供します。初心者向けのチュートリアルから上級者向けの複雑なチャレンジまで、様々な用途に対応できます。

シナリオの作成は比較的簡単で、既存のテンプレートを参考にすれば、独自のシナリオを作成できます。コミュニティでシナリオを共有して、Simutrans の楽しみ方を広げましょう！
