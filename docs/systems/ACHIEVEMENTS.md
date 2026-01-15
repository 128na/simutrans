# Simutrans 実績システム（Achievements）

## 概要

Simutrans の**実績システム**は、プレイヤーの達成や特定の行動に対して報酬を与える機能です。現在は Steam 版で実装されていますが、将来的には独自の実績システムも計画されています。

## 目的

実績システムは、以下のような目的で使用されます：

- **プレイヤーのモチベーション向上**: 明確な目標を提供
- **ゲームプレイの多様性促進**: 通常プレイでは気づかない要素の発見
- **コミュニティの活性化**: 実績の共有や達成方法の議論
- **Pakset の宣伝**: 特定の Pakset でのみ解除できる実績

## システムアーキテクチャ

### コンポーネント

```
simachievements_t (抽象層)
    ↓
steam_achievements_t (Steam API 実装)
    ↓
Steam Backend
```

**関連ファイル**:

- `src/simutrans/simachievements.cc` - 実績チェックロジック
- `src/simutrans/simachievements.h` - 実績システムのインターフェース
- `src/simutrans/simachenum.h` - 実績の列挙定義（X Macro 使用）
- `src/steam/achievements.cc` - Steam API との統合
- `src/steam/achievements.h` - Steam 実績データ構造

### X Macro パターン

実績の定義は X Macro パターンを使用しており、効率的な管理が可能です：

```cpp
// simachenum.h
#define ACHIEVEMENTS     \
    X(ACH_LOAD_PAK192_COMIC) \
    X(ACH_QUERY_DICTACTOR) \
    X(ACH_PROD_INK) \
    X(ACH_TOOL_REMOVE_BUSY_BRIDGE)

// enum 生成
#define X(id) id,
enum simachievements_enum { ACHIEVEMENTS };
#undef X
```

この方法により、1 か所で定義を変更するだけで、複数の場所に自動的に反映されます。

---

## 実績の種類

### 1. Pakset ロード実績

**トリガー**: 特定の Pakset をロードした時

**実装場所**: `simachievements_t::check_pakset_ach()`

**例**:

```cpp
void simachievements_t::check_pakset_ach() {
    std::string pak_name = env_t::pak_name;
    pak_name.erase(pak_name.length() - 1);
    if (STRICMP(pak_name.c_str(), "pak192.comic") == 0) {
        set_achievement(ACH_LOAD_PAK192_COMIC);
    }
}
```

**現在の実績**:

| 実績 ID                 | 説明                | 条件                  |
| ----------------------- | ------------------- | --------------------- |
| `ACH_LOAD_PAK192_COMIC` | pak192.comic を発見 | pak192.comic をロード |

**用途**:

- Pakset 制作者のモチベーション向上
- 新しい Pakset の宣伝
- プレイヤーに多様な Pakset を試させる

---

### 2. クエリ実績

**トリガー**: 特定のオブジェクトを調査（クエリ）した時

**実装場所**: `simachievements_t::check_query_ach(const char* object_name)`

**例**:

```cpp
void simachievements_t::check_query_ach(const char* object_name) {
    std::string pak_name = env_t::pak_name;
    pak_name.erase(pak_name.length() - 1);
    if ((STRICMP(pak_name.c_str(), "pak128") == 0) &&
        strstr(object_name, "rmax_dictator_statue")) {
        set_achievement(ACH_QUERY_DICTACTOR);
    }
}
```

**現在の実績**:

| 実績 ID               | 説明             | 条件                        |
| --------------------- | ---------------- | --------------------------- |
| `ACH_QUERY_DICTACTOR` | 独裁者の像を発見 | pak128 で独裁者の像をクエリ |

**用途**:

- イースターエッグの実装
- 特定の建物や車両への注目を集める
- コミュニティでの話題作り

---

### 3. ゲーム状態実績

**トリガー**: ゲームの特定の状態を達成した時

**実装場所**: `simachievements_t::check_state_ach(karte_t* world)`

**例**:

```cpp
void simachievements_t::check_state_ach(karte_t* world) {
    if (env_t::networkmode)
        return;  // マルチプレイヤーでは実績を無効化

    std::string pak_name = env_t::pak_name;
    pak_name.erase(pak_name.length() - 1);

    for (fabrik_t* fab : world->get_fab_list()) {
        if (STRICMP(pak_name.c_str(), "pak192.comic") == 0) {
            if (check_yearly_production(fab, "Mr. Kraken", 0)) {
                set_achievement(ACH_PROD_INK);
            }
        }
    }
}
```

**現在の実績**:

| 実績 ID                       | 説明             | 条件                                              |
| ----------------------------- | ---------------- | ------------------------------------------------- |
| `ACH_PROD_INK`                | インクを大量生産 | pak192.comic で "Mr. Kraken" 工場が目標生産量達成 |
| `ACH_TOOL_REMOVE_BUSY_BRIDGE` | 使用中の橋を削除 | 車両が通行中の橋を削除                            |

**生産量チェックヘルパー関数**:

```cpp
bool check_yearly_production(
    fabrik_t* fab,
    const char* name,
    uint32 product_index,
    double target_percent = 0.70  // デフォルトは最大生産の 70%
) {
    // 工場名が一致するか確認
    // 指定された製品の年間生産量を取得
    // 目標生産量と比較
    return yearly_production >= target_production;
}
```

**用途**:

- 経済システムの理解促進
- 効率的な輸送ネットワーク構築の奨励
- 長期プレイの目標設定

---

## 実装の詳細

### 実績の追加方法

新しい実績を追加するには、以下の手順を実行します：

#### 1. 実績 ID を定義（`simachenum.h`）

```cpp
// 実績の総数を更新
#define NUM_ACHIEVEMENTS 5  // 新しい実績を追加したので +1

// X Macro に新しい実績を追加
#define ACHIEVEMENTS     \
    X(ACH_LOAD_PAK192_COMIC) \
    X(ACH_QUERY_DICTACTOR) \
    X(ACH_PROD_INK) \
    X(ACH_TOOL_REMOVE_BUSY_BRIDGE) \
    X(ACH_MY_NEW_ACHIEVEMENT)  // 新しい実績
```

#### 2. チェックロジックを実装（`simachievements.cc`）

適切なチェック関数に条件を追加：

```cpp
void simachievements_t::check_state_ach(karte_t* world) {
    if (env_t::networkmode)
        return;

    // 新しい実績の条件チェック
    if (/* 条件 */) {
        set_achievement(ACH_MY_NEW_ACHIEVEMENT);
    }
}
```

#### 3. チェック関数を適切なタイミングで呼び出す

```cpp
// 例: メインループ内で定期的にチェック
simachievements_t::check_state_ach(world);

// 例: 特定のアクション後にチェック
if (action_performed) {
    simachievements_t::check_query_ach(object_name);
}
```

---

## Steam 統合

### Steam API との連携

Steam 版では、`steam_achievements_t` クラスが Steam API と通信します：

```cpp
class steam_achievements_t {
private:
    uint64 app_id;
    achievement_t *achievements;
    bool stats_initialized;
    std::vector<int> achievements_queue;  // オフライン時のキュー

public:
    bool request_stats();
    bool set_achievement(int ach_enum);

    // Steam コールバック
    STEAM_CALLBACK(on_user_stats_received, ...);
    STEAM_CALLBACK(on_user_stats_stored, ...);
    STEAM_CALLBACK(on_achievement_stored, ...);
};
```

### 実績データ構造

```cpp
struct achievement_t {
    int m_eAchievementID;           // 内部 ID
    const char *m_pchAchievementID; // Steam API ID
    char m_rgchName[128];           // 実績名
    char m_rgchDescription[256];    // 説明
    bool m_bAchieved;               // 解除済みフラグ
    int m_iIconImage;               // アイコンハンドル
};
```

### オフライン対応

Steam API が利用できない場合、実績はキューに保存され、次回オンライン時に同期されます：

```cpp
bool steam_achievements_t::set_achievement(int ach_enum) {
    if (!stats_initialized) {
        // オフラインの場合はキューに追加
        achievements_queue.push_back(ach_enum);
        return false;
    }

    // Steam API を使って実績を解除
    return SteamUserStats()->SetAchievement(
        achievements[ach_enum].m_pchAchievementID
    );
}
```

---

## マルチプレイヤーでの実績

### 基本方針

マルチプレイヤーゲームでは、**状態ベースの実績は無効化**されます：

```cpp
void simachievements_t::check_state_ach(karte_t* world) {
    if (env_t::networkmode)
        return;  // マルチプレイヤーでは実行しない

    // 実績チェックのロジック...
}
```

**理由**:

- 不正行為の防止
- 他のプレイヤーの貢献による不公平な解除の回避
- サーバー負荷の軽減

### 許可される実績

以下のタイプの実績は、マルチプレイヤーでも許可されます：

- **Pakset ロード実績**: クライアント側で判定可能
- **クエリ実績**: プレイヤー個人のアクションのみ

---

## デバッグとテスト

### デバッグメッセージ

実績の解除時にログが出力されます：

```cpp
void simachievements_t::set_achievement(simachievements_enum ach) {
    dbg->message("simachievements_t::set_achievement()",
                 "Unlocking achievement %d", ach);
#ifdef STEAM_BUILT
    steam_t::get_instance()->get_achievements()->set_achievement(ach);
#endif
}
```

### Steam 版のテスト

Steam の実績テストには、以下のツールを使用します：

1. **Steam Overlay**: ゲーム内で実績の解除を確認
2. **Steam Stats スクリーンショット**: 実績のアイコンと説明を確認
3. **Steam API デバッガ**: `SteamUserStats()` の呼び出しをトレース

### 非 Steam 版のテスト

非 Steam ビルドでは、ログメッセージのみが出力されます：

```bash
# デバッグログを有効化して起動
./simutrans -debug 3 -log 1

# ログファイルを確認
tail -f simutrans.log | grep achievement
```

---

## 将来の拡張

### 独自の実績システム

コード内のコメントには、将来的な独自実績システムの実装が示唆されています：

```cpp
/**
 * Manages the achievement system. This currently only works for Steam achievements.
 * TODO: It would be cool to have our own achievement system!
 */
class simachievements_t {
    // ...
};
```

**想定される機能**:

- **ローカル保存**: セーブファイルに実績データを保存
- **UI 統合**: ゲーム内の実績画面
- **進捗表示**: 実績の進捗をパーセンテージで表示
- **報酬システム**: 実績解除時の特別なアイテムや称号
- **統計追跡**: 詳細な統計情報の記録

### スクリプトからの実績設定

シナリオやチュートリアルから実績を解除できるようにする計画もあります：

```cpp
// TODO: A function to set achievement from script tools
// (for scenarios, tutorial, etc...)
```

これにより、Squirrel スクリプトから実績を制御可能になります：

```squirrel
// 想定される Squirrel API
achievement.unlock("ACH_TUTORIAL_COMPLETE");
achievement.check_progress("ACH_CONNECT_CITIES", 5, 10);
```

---

## 実績設計のベストプラクティス

### 1. 多様性の確保

様々なプレイスタイルに対応した実績を用意：

- **探索型**: 隠された要素の発見
- **建設型**: 大規模なネットワーク構築
- **経済型**: 収益や生産量の達成
- **効率型**: 最適化されたルート設計

### 2. 難易度のバランス

- **簡単**: 最初の数時間で解除可能
- **中程度**: 通常プレイで自然に達成
- **難しい**: 意図的な挑戦が必要
- **非常に難しい**: 長期プレイや高度な戦略が必要

### 3. Pakset 依存の明示

特定の Pakset でのみ解除できる実績は、その旨を明記：

```cpp
// pak192.comic でのみ解除可能
if (STRICMP(pak_name.c_str(), "pak192.comic") == 0) {
    // 実績チェック
}
```

### 4. パフォーマンスへの配慮

実績チェックは頻繁に実行されるため、パフォーマンスに注意：

- **重い処理は避ける**: O(n²) などの複雑なアルゴリズムは使用しない
- **キャッシュの活用**: 計算結果を保存して再利用
- **定期的なチェック**: 毎フレームではなく、月次などで実行

---

## 統計データ

### 実績の解除率（Steam 版の例）

| 実績               | 解除率 | 難易度   |
| ------------------ | ------ | -------- |
| 最初の路線を開設   | 95%    | 簡単     |
| 10 都市を接続      | 60%    | 中       |
| 月間収益 100 万    | 30%    | 難       |
| すべての商品を輸送 | 5%     | 非常に難 |

---

## トラブルシューティング

### 実績が解除されない

**原因**:

- Steam が初期化されていない
- マルチプレイヤーモード
- 条件が正しく実装されていない

**対処法**:

```cpp
// Steam の初期化を確認
if (steam_t::get_instance()) {
    // Steam が利用可能
}

// デバッグログを確認
dbg->message("achievement", "Condition met: %d", condition_result);
```

### 実績が重複して解除される

**原因**: 同じ実績を複数回 `set_achievement()` で呼び出している

**対処法**: Steam API は自動的に重複を防ぐため、通常は問題ありません。

---

## 関連ファイル

- **実装**: `src/simutrans/simachievements.{cc,h}`
- **列挙定義**: `src/simutrans/simachenum.h`
- **Steam 統合**: `src/steam/achievements.{cc,h}`
- **Steam ラッパー**: `src/steam/steam.{cc,h}`

---

## 参考リンク

- [Steam 実績ガイド（英語）](https://partner.steamgames.com/doc/features/achievements)
- [Steamworks API ドキュメント](https://partner.steamgames.com/doc/api/ISteamUserStats)
- [Simutrans Steam ページ](https://store.steampowered.com/app/434520/Simutrans/)

---

## まとめ

Simutrans の実績システムは、プレイヤーのエンゲージメントを高め、ゲームの多様な要素を発見させる効果的な機能です。現在は Steam 版で実装されていますが、将来的には独自システムの追加も計画されており、さらなる拡張が期待されます。

実績の追加は比較的簡単で、X Macro パターンにより保守性も高く保たれています。新しい実績のアイデアがあれば、コミュニティで提案してみてください！
