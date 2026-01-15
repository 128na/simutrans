# Simutrans プレイヤー管理システム

## 概要

Simutrans の**プレイヤー管理システム**は、ゲーム内の複数の企業（プレイヤー）を管理するためのシステムです。人間プレイヤー、AI プレイヤー、公共事業プレイヤーを統一的に扱い、財務管理、権限制御、ネットワーク同期などを提供します。

## 目的

プレイヤー管理システムは、以下のような用途で使用されます：

- **マルチプレイヤー対応**: 複数の人間プレイヤーの管理
- **AI プレイヤー**: コンピュータ制御の競合企業
- **公共事業**: 道路や公共交通の管理
- **権限制御**: パスワード保護、ロック/アンロック
- **財務管理**: 収支、資産、倒産判定
- **シナリオ連携**: 進捗追跡、目標達成

## システムアーキテクチャ

### コンポーネント

```
karte_t (ワールド)
    ↓
player_t[MAX_PLAYER_COUNT] (プレイヤー配列)
    ↓
finance_t (財務管理)
simlinemgmt_t (路線管理)
```

**関連ファイル**:

- `src/simutrans/player/simplay.{cc,h}` - プレイヤーの実装
- `src/simutrans/player/finance.{cc,h}` - 財務システム
- `src/simutrans/world/simworld.{cc,h}` - ワールドとの連携
- `src/simutrans/dataobj/scenario.{cc,h}` - シナリオとの連携

---

## プレイヤータイプ

### 基本タイプ

```cpp
enum {
    EMPTY        = 0,  // 空きスロット
    HUMAN        = 1,  // 人間プレイヤー
    AI_GOODS     = 2,  // 貨物輸送 AI
    AI_PASSENGER = 3,  // 旅客輸送 AI
    AI_SCRIPTED  = 4,  // スクリプト制御 AI
    MAX_AI,
    PASSWORD_PROTECTED = 128  // パスワード保護フラグ
};
```

### プレイヤー番号

| プレイヤー番号 | 説明                 | 定数               |
| -------------- | -------------------- | ------------------ |
| 0              | 人間プレイヤー（主） | `PLAYER_HUMAN_NR`  |
| 1              | 公共事業プレイヤー   | `PLAYER_PUBLIC_NR` |
| 2-15           | 追加プレイヤー（AI） | -                  |

**最大プレイヤー数**: `MAX_PLAYER_COUNT` (通常 16)

---

## プレイヤークラスの構造

### 主要メンバー変数

```cpp
class player_t {
protected:
    char player_name_buf[256];           // プレイヤー名
    finance_t *finance;                  // 財務データ
    uint16 player_age;                   // 企業年齢

    uint8 player_color_1, player_color_2;  // カラースキーム
    uint8 player_nr;                     // プレイヤー番号

    bool active;                         // AI として有効か
    bool locked;                         // ロック状態
    bool unlock_pending;                 // アンロック待機中
    pwd_hash_t pwd_hash;                 // パスワードハッシュ

    sint32 headquarter_level;            // 本社レベル
    koord headquarter_pos;               // 本社位置

    sint32 scenario_completion;          // シナリオ達成率

public:
    simlinemgmt_t simlinemgmt;          // 路線管理
};
```

---

## 財務管理

### finance_t クラス

各プレイヤーは独自の財務オブジェクトを持ちます：

```cpp
class finance_t {
private:
    player_t *player;                    // 所有プレイヤー
    sint64 account_balance;              // 口座残高
    sint64 account_overdrawn;            // 借金期間（月数）

    // 財務履歴（過去 12 ヶ月 + 過去 12 年）
    sint64 finance_history_year[MAX_HISTORY_YEARS][MAX_HISTORY_TYPES];
    sint64 finance_history_month[MAX_HISTORY_MONTHS][MAX_HISTORY_TYPES];
};
```

### 財務履歴の種類

| タイプ                  | 説明             |
| ----------------------- | ---------------- |
| `CONSTRUCTION_COST`     | 建設費用         |
| `VEHICLE_REVENUE`       | 車両収益         |
| `VEHICLE_RUNNING_COSTS` | 車両運行費       |
| `WAY_TOLL`              | 道路使用料       |
| `POWERLINES`            | 送電線収益・費用 |
| `OPERATIONS_PROFIT`     | 営業利益         |
| `MAINTENANCE`           | 維持費           |
| `ASSETS`                | 総資産           |
| `CASH`                  | 現金             |
| `NET_WEALTH`            | 純資産           |
| `PROFIT`                | 純利益           |

### 財務メソッド

```cpp
// 口座残高の操作
void book_account(sint64 amount);
sint64 get_account_balance() const;
double get_account_balance_as_double() const;

// 借金状態のチェック
int get_account_overdrawn() const;
bool has_money_or_assets() const;
bool can_afford(sint64 cost) const;

// 履歴の取得
sint64 get_finance_history_year(int year, int type) const;
sint64 get_finance_history_month(int month, int type) const;

// 資産の更新
void calc_assets();
void update_assets(sint64 delta, waytype_t wt);
```

---

## 倒産システム

### 倒産条件

プレイヤーは以下の条件で倒産します：

1. **純資産がマイナス**（`NET_WEALTH < 0`）
2. **3 ヶ月連続**で純資産がマイナス

### 倒産判定

```cpp
bool player_t::new_month() {
    // 財務更新
    finance->new_month();

    // 純資産がマイナスか確認
    sint64 net_wealth = finance->get_net_wealth();

    if (net_wealth < 0) {
        account_overdrawn++;

        if (account_overdrawn >= 3) {
            // 3ヶ月連続でマイナス → 倒産
            ai_bankrupt();
            return false;  // プレイヤーを削除
        }
    } else {
        account_overdrawn = 0;
    }

    return true;  // 継続
}
```

### 倒産処理

```cpp
void player_t::ai_bankrupt() {
    // すべての資産を削除
    // - 車両を売却
    // - 停留所を削除
    // - 路線を削除
    // - インフラを削除

    // プレイヤーをロック
    locked = true;

    // メッセージを表示
    world->get_message()->add_message(
        player_name_buf + " has gone bankrupt!",
        koord::invalid,
        message_t::general
    );
}
```

---

## 権限管理

### ロック/アンロックシステム

プレイヤーは以下の状態を持ちます：

```cpp
bool is_locked() const { return locked; }
bool is_unlock_pending() const { return unlock_pending; }

void unlock(bool unlock_, bool unlock_pending_ = false) {
    locked = !unlock_;
    unlock_pending = unlock_pending_;
}
```

**ロック状態の意味**:

- **ロック**: プレイヤーは操作できない（AI も停止）
- **アンロック待機**: パスワード認証待ち（マルチプレイヤー）

### パスワード保護

```cpp
class player_t {
private:
    pwd_hash_t pwd_hash;  // SHA1 ハッシュ

public:
    void check_unlock(const pwd_hash_t& hash) {
        if (pwd_hash == hash) {
            unlock(true);
        }
    }

    pwd_hash_t& access_password_hash() {
        return pwd_hash;
    }
};
```

**パスワードハッシュの保存**:

```cpp
class karte_t {
private:
    pwd_hash_t player_password_hash[MAX_PLAYER_COUNT];

public:
    void rdwr_player_password_hashes(loadsave_t *file) {
        for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
            player_password_hash[i].rdwr(file);
        }
    }
};
```

---

## 本社（Headquarters）システム

### 本社の役割

- **企業のシンボル**: プレイヤーの存在を視覚的に表現
- **レベルシステム**: 企業の成長に応じてアップグレード
- **ボーナス**: 高レベルの本社は特別なボーナスを提供（実装依存）

### 本社の管理

```cpp
class player_t {
private:
    sint32 headquarter_level;   // 0 = なし, 1-5 = レベル
    koord headquarter_pos;      // マップ上の位置

public:
    void add_headquarter(short hq_level, koord hq_pos) {
        headquarter_level = hq_level;
        headquarter_pos = hq_pos;
    }

    koord get_headquarter_pos() const {
        return headquarter_pos;
    }

    short get_headquarter_level() const {
        return headquarter_level;
    }
};
```

### 本社の建設

```cpp
// ツールから呼び出される
tool_t::work() {
    // 本社建設地点を選択
    koord pos = get_selected_position();

    // プレイヤーに本社を追加
    player->add_headquarter(1, pos);

    // 建物オブジェクトを配置
    gebaeude_t* hq = new gebaeude_t(pos, player, desc);
    world->set_building(pos, hq);
}
```

---

## メッセージシステム

### 収入メッセージ

プレイヤーは画面上に浮かぶメッセージを表示できます：

```cpp
class player_t {
private:
    class income_message_t {
    public:
        koord pos;              // 表示位置
        sint64 amount;          // 金額
        uint32 alter;           // 経過時間

        void * operator new(size_t);
        void operator delete(void *p);
    };

    slist_tpl<income_message_t *> messages;

public:
    void add_money_message(sint64 amount, koord k);
    void display_messages();
    void age_messages(uint32 delta_t);
};
```

### メッセージの表示

```cpp
void player_t::display_messages() {
    for (income_message_t* msg : messages) {
        if (msg->alter < MAX_AGE) {
            // 画面座標に変換
            scr_coord screen_pos = world->get_viewport()->get_screen_coord(msg->pos);

            // 金額をフォーマット
            char buffer[64];
            money_to_string(buffer, msg->amount);

            // 画面に描画
            display_proportional_clip(
                screen_pos.x, screen_pos.y,
                buffer,
                ALIGN_LEFT,
                msg->amount >= 0 ? COL_GREEN : COL_RED,
                true
            );
        }
    }
}
```

---

## AI プレイヤー

### AI の基本動作

AI プレイヤーは `player_t` から派生したクラスで実装されます：

```cpp
class ai_goods_t : public player_t {
public:
    virtual void step() override;      // 毎フレーム呼ばれる
    virtual bool new_month() override; // 月次処理
    virtual uint8 get_ai_id() const override { return AI_GOODS; }
};

class ai_passenger_t : public player_t {
public:
    virtual void step() override;
    virtual bool new_month() override;
    virtual uint8 get_ai_id() const override { return AI_PASSENGER; }
};
```

### AI の思考ルーチン

```cpp
void ai_goods_t::step() {
    if (!active) return;

    // 1. 収益性の高いルートを探す
    find_profitable_routes();

    // 2. 車両を購入・配置
    buy_and_deploy_vehicles();

    // 3. 赤字路線を削除
    remove_unprofitable_routes();

    // 4. インフラを拡張
    expand_infrastructure();
}
```

---

## シナリオとの連携

### 達成率の追跡

```cpp
class player_t {
private:
    sint32 scenario_completion;  // 0-100

public:
    sint32 get_scenario_completion() const {
        return scenario_completion;
    }

    void set_scenario_completion(sint32 percent) {
        scenario_completion = clamp(percent, 0, 100);
    }
};
```

### シナリオからの使用

```squirrel
// Squirrel スクリプト
function step() {
    local player = world.get_player(0)
    local completion = calculate_player_progress(player)
    player.set_scenario_completion(completion)
}
```

---

## ネットワーク同期

### パスワードハッシュの同期

マルチプレイヤーでは、パスワードハッシュがクライアント間で同期されます：

```cpp
void karte_t::rdwr_player_password_hashes(loadsave_t *file) {
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
        player_password_hash[i].rdwr(file);
    }
}

const pwd_hash_t& karte_t::get_player_password_hash(uint8 player_nr) const {
    return player_password_hash[player_nr];
}

void karte_t::clear_player_password_hashes() {
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
        player_password_hash[i].clear();
    }
}
```

### プレイヤーの追加・削除

```cpp
void karte_t::call_change_player_tool(
    uint8 cmd,
    uint8 player_nr,
    uint16 param,
    bool scripted_call
) {
    enum change_player_tool_cmds {
        new_player = 1,
        toggle_freeplay,
        delete_player,
        // ...
    };

    switch (cmd) {
        case new_player:
            // 新しいプレイヤーを作成
            create_new_player(player_nr, param);
            break;

        case delete_player:
            // プレイヤーを削除
            remove_player(player_nr);
            break;
    }
}
```

---

## 公共事業プレイヤー

### 特別な役割

公共事業プレイヤー（player 1）は特別な役割を持ちます：

```cpp
bool player_t::is_public_service() const {
    return player_nr == PLAYER_PUBLIC_NR;
}

player_t* karte_t::get_public_player() const {
    return players[PLAYER_PUBLIC_NR];
}
```

**機能**:

- 公共道路の管理
- 初期インフラの提供
- 維持費が無料または低額
- 倒産しない

### 公共道路の管理

```cpp
// 道路を建設する際
if (builder->is_public_road()) {
    // 公共事業プレイヤーが所有
    owner = world->get_public_player();
} else {
    // アクティブプレイヤーが所有
    owner = world->get_active_player();
}
```

---

## Undo システム

### Undo バッファ

プレイヤーは最近の操作を取り消すことができます：

```cpp
class player_t {
private:
    waytype_t undo_type;                      // 対象の Waytype
    vector_tpl<koord3d> last_built;           // 建設リスト

public:
    void init_undo(waytype_t wtype, unsigned short max);
    void add_undo(koord3d k);
    sint64 undo();
};
```

### Undo の実行

```cpp
sint64 player_t::undo() {
    sint64 cost = 0;

    // 逆順で削除
    while (!last_built.empty()) {
        koord3d pos = last_built.pop_back();

        // 建設物を削除
        cost += remove_construction_at(pos);
    }

    return cost;
}
```

---

## カラースキーム

### プレイヤーカラー

各プレイヤーは 2 色のカラースキームを持ちます：

```cpp
class player_t {
private:
    uint8 player_color_1, player_color_2;

public:
    uint8 get_player_color1() const { return player_color_1; }
    uint8 get_player_color2() const { return player_color_2; }

    void set_player_color(uint8 col1, uint8 col2) {
        player_color_1 = col1;
        player_color_2 = col2;
    }
};
```

### カラーの用途

- **車両**: プレイヤーカラーで塗装
- **建物**: 屋根や壁の色
- **UI**: ウィンドウの枠線
- **マップ**: 所有物の表示色

### デフォルトカラーの設定

```cpp
void settings_t::set_player_color_to_default(player_t* player) {
    static const uint8 default_colors[][2] = {
        {28, 255},  // Player 0: Blue
        {86, 255},  // Player 1 (Public): Yellow
        {135, 255}, // Player 2: Red
        {76, 255},  // Player 3: Purple
        // ...
    };

    uint8 nr = player->get_player_nr();
    player->set_player_color(
        default_colors[nr][0],
        default_colors[nr][1]
    );
}
```

---

## プレイヤーの切り替え

### アクティブプレイヤー

```cpp
class karte_t {
private:
    player_t *active_player;        // 現在操作中のプレイヤー
    uint8 active_player_nr;         // プレイヤー番号

public:
    player_t* get_active_player() const {
        return active_player;
    }

    uint8 get_active_player_nr() const {
        return active_player_nr;
    }

    void switch_active_player(uint8 nr) {
        if (nr < MAX_PLAYER_COUNT && players[nr]) {
            active_player = players[nr];
            active_player_nr = nr;
        }
    }
};
```

---

## セーブ/ロード

### プレイヤーデータの保存

```cpp
void player_t::rdwr(loadsave_t *file) {
    // 基本情報
    file->rdwr_str(player_name_buf);
    file->rdwr_short(player_age);
    file->rdwr_byte(player_nr);
    file->rdwr_byte(player_color_1);
    file->rdwr_byte(player_color_2);

    // 財務データ
    finance->rdwr(file);

    // 本社情報
    headquarter_pos.rdwr(file);
    file->rdwr_long(headquarter_level);

    // パスワードハッシュ
    pwd_hash.rdwr(file);

    // 路線管理
    simlinemgmt.rdwr(file);

    // シナリオ進捗
    file->rdwr_long(scenario_completion);
}
```

---

## デバッグとテスト

### デバッグ出力

```cpp
void player_t::debug_print() {
    dbg->message("player_t", "Player %d: %s",
                 player_nr, player_name_buf);
    dbg->message("  Balance: %lld",
                 finance->get_account_balance());
    dbg->message("  Net Wealth: %lld",
                 finance->get_net_wealth());
    dbg->message("  Active: %d, Locked: %d",
                 active, locked);
}
```

### テストコマンド

```cpp
// デバッグコンソールから
debug_player(0)  // プレイヤー 0 の情報を表示
set_money(0, 1000000)  // プレイヤー 0 に資金を追加
lock_player(2)  // プレイヤー 2 をロック
```

---

## パフォーマンスの考慮事項

### 最適化のポイント

1. **財務計算**: 毎月 1 回のみ実行
2. **AI 思考**: フレームごとに一部のみ実行
3. **メッセージ**: 古いメッセージは自動削除
4. **キャッシュ**: 頻繁にアクセスされるデータをキャッシュ

```cpp
// 良い例: 月次処理
bool player_t::new_month() {
    // 重い計算は月に1回
    finance->calc_assets();
    finance->new_month();
    return true;
}

// 悪い例: 毎フレーム重い処理
void player_t::step() {
    // これは避ける
    finance->calc_assets();  // 重い！
}
```

---

## 関連ファイル

- **プレイヤー管理**: `src/simutrans/player/simplay.{cc,h}`
- **財務システム**: `src/simutrans/player/finance.{cc,h}`
- **AI 実装**: `src/simutrans/player/ai_{goods,passenger,scripted}.{cc,h}`
- **ワールド統合**: `src/simutrans/world/simworld.{cc,h}`

---

## まとめ

Simutrans のプレイヤー管理システムは、人間プレイヤー、AI、公共事業を統一的に扱う包括的なシステムです。財務管理、権限制御、ネットワーク同期など、多様な機能を提供しており、シングルプレイヤーからマルチプレイヤー、シナリオまで幅広く対応しています。

プレイヤーシステムの理解は、Simutrans の内部構造を把握する上で重要であり、MOD 開発や AI の実装にも役立ちます。
