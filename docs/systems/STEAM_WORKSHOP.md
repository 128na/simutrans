# Steam Workshop 統合

## 概要

Simutrans の **Steam Workshop 統合**は、Steam 版において、コミュニティが作成した Pakset、シナリオ、マップ、アドオンを簡単にダウンロード・インストールできる機能です。Steam のクラウドインフラを活用して、コンテンツの配布と管理を自動化します。

## 目的

Steam Workshop 統合は、以下のような用途で使用されます：

- **コンテンツの簡単な配布**: クリックひとつでインストール
- **自動更新**: アップデートの自動ダウンロード
- **コミュニティの活性化**: 作品の共有とフィードバック
- **バージョン管理**: 複数バージョンの管理
- **依存関係の解決**: 必要な Pakset の自動インストール

## システムアーキテクチャ

### コンポーネント

```
Steam Workshop API
    ↓
workshop_item_t (アイテム管理)
    ↓
steam_t (Steam ラッパー)
    ↓
Simutrans ファイルシステム
```

**関連ファイル**:

- `src/steam/workshop_item.{cc,h}` - Workshop アイテムの管理
- `src/steam/steam.{cc,h}` - Steam API のラッパー
- `src/simutrans/dataobj/pakset_manager.{cc,h}` - Pakset の読み込み
- `src/simutrans/gui/pakset_downloader_t.{cc,h}` - ダウンロード UI

---

## Workshop アイテムの種類

### カテゴリタグ

Workshop アイテムは以下のカテゴリに分類されます：

| カテゴリ     | 説明                             | タグ名     |
| ------------ | -------------------------------- | ---------- |
| **Pakset**   | ゲームグラフィックとオブジェクト | `pakset`   |
| **Scenario** | シナリオ/ミッション              | `scenario` |
| **Map**      | カスタムマップ                   | `map`      |
| **Addon**    | 追加コンテンツ                   | `addon`    |

### Pakset タグ

Pakset には追加のタグを付けることができます：

- `pak64` - 64x64 タイルサイズ
- `pak128` - 128x128 タイルサイズ
- `pak192.comic` - 192x192 (comic スタイル)
- `pak256` - 256x256 タイルサイズ
- その他のカスタム Pakset 名

---

## workshop_item_t クラス

### クラス定義

```cpp
class workshop_item_t {
private:
    PublishedFileId_t id;                  // Steam Workshop ID
    std::string title;                     // アイテムタイトル
    std::vector<std::string> paths;        // ローカルパス
    std::vector<std::string> files;        // ファイルリスト
    std::string category_tag;              // カテゴリタグ
    std::vector<std::string> pakset_tags;  // Pakset タグ

    void fill_paths();  // パスを自動検出

public:
    workshop_item_t(PublishedFileId_t id, std::vector<std::string> files);
    workshop_item_t(PublishedFileId_t id, std::string title);

    bool set_category_tag(std::string tag);
    bool add_pakset_tag(std::string tag);
    bool install(std::string origin);
    void uninstall();

    PublishedFileId_t get_id() { return id; }
    std::vector<std::string> get_files() { return files; }
};
```

---

## インストールプロセス

### 1. Workshop アイテムのダウンロード

Steam API が自動的にダウンロードを処理します：

```cpp
// Steam が Workshop アイテムをダウンロード
// ダウンロード先: Steam/steamapps/workshop/content/434520/<item_id>/
```

### 2. カテゴリの判定

```cpp
bool workshop_item_t::set_category_tag(std::string tag) {
    // 有効なカテゴリタグのみ許可
    if (tag == "pakset" || tag == "scenario" ||
        tag == "map" || tag == "addon") {
        category_tag = tag;
        return true;
    }
    return false;
}
```

### 3. ファイルのインストール

```cpp
bool workshop_item_t::install(std::string origin) {
    // カテゴリに応じてインストール先を決定
    std::string dest_dir;

    if (category_tag == "pakset") {
        dest_dir = env_t::user_dir + "/pak/";
    } else if (category_tag == "scenario") {
        dest_dir = env_t::user_dir + "/scenario/";
    } else if (category_tag == "map") {
        dest_dir = env_t::user_dir + "/maps/";
    } else if (category_tag == "addon") {
        dest_dir = env_t::user_dir + "/addons/" + env_t::pak_name + "/";
    }

    // ファイルをコピー
    for (const auto& file : files) {
        copy_file(origin + "/" + file, dest_dir + file);
    }

    return true;
}
```

---

## ディレクトリ構造

### Workshop ダウンロード先

```
Steam/steamapps/workshop/content/434520/
└── <workshop_item_id>/
    ├── pak/              # Pakset ファイル
    ├── config/           # 設定ファイル
    ├── text/             # 翻訳ファイル
    └── scenario/         # シナリオファイル
```

### Simutrans インストール先

```
<user_dir>/
├── pak/                  # Pakset (Workshop からインストール)
│   └── pak128.workshop/
├── scenario/             # シナリオ
│   └── my_scenario/
├── maps/                 # カスタムマップ
│   └── island.sve
└── addons/               # アドオン
    └── pak64/
        └── addon_vehicles/
```

---

## パスの自動検出

### fill_paths() メソッド

Workshop アイテムから必要なファイルを自動的に検出します：

```cpp
void workshop_item_t::fill_paths() {
    // Steam Workshop のダウンロードディレクトリ
    std::string base_path = get_workshop_download_path(id);

    // カテゴリに応じたファイルを検索
    if (category_tag == "pakset") {
        // .pak ファイルを検索
        find_files_recursive(base_path, "*.pak", files);

        // config, text ディレクトリも含める
        if (directory_exists(base_path + "/config"))
            paths.push_back(base_path + "/config");
        if (directory_exists(base_path + "/text"))
            paths.push_back(base_path + "/text");

    } else if (category_tag == "scenario") {
        // .nut スクリプトと .sve セーブファイルを検索
        find_files_recursive(base_path, "*.nut", files);
        find_files_recursive(base_path, "*.sve", files);
    }
}
```

---

## アンインストール

### uninstall() メソッド

```cpp
void workshop_item_t::uninstall() {
    // インストール先のディレクトリを削除
    std::string install_dir;

    if (category_tag == "pakset") {
        install_dir = env_t::user_dir + "/pak/" + title + ".workshop/";
    } else if (category_tag == "scenario") {
        install_dir = env_t::user_dir + "/scenario/" + title + "/";
    }

    // ディレクトリを再帰的に削除
    remove_directory_recursive(install_dir);

    // Workshop 購読を解除
    SteamUGC()->UnsubscribeItem(id);
}
```

---

## Steam API との連携

### 購読イベントの処理

```cpp
class steam_t {
private:
    // Workshop アイテムがダウンロードされた時のコールバック
    STEAM_CALLBACK(steam_t, on_item_downloaded, DownloadItemResult_t);

    // Workshop アイテムがインストールされた時のコールバック
    STEAM_CALLBACK(steam_t, on_item_installed, ItemInstalled_t);
};

void steam_t::on_item_downloaded(DownloadItemResult_t* callback) {
    if (callback->m_eResult == k_EResultOK) {
        // アイテムを自動的にインストール
        PublishedFileId_t item_id = callback->m_nPublishedFileId;
        install_workshop_item(item_id);
    }
}
```

### アイテム情報の取得

```cpp
void get_workshop_item_info(PublishedFileId_t item_id) {
    // Steam API からアイテム情報を取得
    uint32 state = SteamUGC()->GetItemState(item_id);

    if (state & k_EItemStateSubscribed) {
        dbg->message("Workshop", "Item %llu is subscribed", item_id);
    }

    if (state & k_EItemStateInstalled) {
        dbg->message("Workshop", "Item %llu is installed", item_id);

        // インストールパスを取得
        char install_path[1024];
        uint64 size_on_disk;
        uint32 timestamp;

        if (SteamUGC()->GetItemInstallInfo(
            item_id,
            &size_on_disk,
            install_path,
            sizeof(install_path),
            &timestamp
        )) {
            dbg->message("Workshop", "Install path: %s", install_path);
        }
    }
}
```

---

## Pakset の検出と読み込み

### Workshop Pakset の優先順位

```cpp
void pakset_manager_t::load_pakset(bool load_addons) {
    std::vector<std::string> pakset_paths;

    // 1. 標準 Pakset
    pakset_paths.push_back(env_t::base_dir + "/pak/");

    // 2. Workshop Pakset
    pakset_paths.push_back(env_t::user_dir + "/pak/");

    // 3. ユーザー Addon (Workshop 含む)
    if (load_addons) {
        pakset_paths.push_back(
            env_t::user_dir + "/addons/" + env_t::pak_name + "/"
        );
    }

    // 各パスから .pak ファイルを読み込み
    for (const auto& path : pakset_paths) {
        load_paks_from_directory(path, load_addons);
    }
}
```

---

## UI 統合

### Workshop ブラウザ

Steam Overlay 内で Workshop を閲覧できます：

```cpp
void open_workshop_browser() {
    // Steam Overlay で Workshop ページを開く
    SteamFriends()->ActivateGameOverlayToWebPage(
        "steam://url/SteamWorkshopPage/434520"
    );
}
```

### ゲーム内からのアクセス

```cpp
// メインメニューに "Workshop" ボタンを追加
void main_menu_t::action_triggered(gui_action_creator_t* comp, value_t extra) {
    if (comp == &workshop_button) {
        open_workshop_browser();
    }
}
```

---

## Workshop アイテムの作成

### アップロードの準備

1. **ディレクトリ構造の作成**

```
my_pakset/
├── pak/
│   ├── vehicles.pak
│   ├── buildings.pak
│   └── ways.pak
├── config/
│   └── simuconf.tab
├── text/
│   ├── en.tab
│   └── ja.tab
└── preview.png
```

2. **メタデータファイルの作成**

```json
{
	"title": "My Custom Pakset",
	"description": "A collection of custom vehicles and buildings",
	"tags": ["pakset", "pak128"],
	"visibility": "public",
	"preview_image": "preview.png"
}
```

### Steam Workshop Uploader の使用

```bash
# Steam Workshop Uploader ツールを使用
steamcmd +login <username> +workshop_build_item <app_id> <metadata_file> +quit
```

または、Steamworks SDK の Web API を使用：

```cpp
void upload_workshop_item() {
    // アイテムを作成
    SteamAPICall_t handle = SteamUGC()->CreateItem(
        434520,  // Simutrans App ID
        k_EWorkshopFileTypeCommunity
    );

    // 結果を待つ
    // on_create_item_result() コールバックで処理
}

void on_create_item_result(CreateItemResult_t* result) {
    if (result->m_eResult == k_EResultOK) {
        PublishedFileId_t item_id = result->m_nPublishedFileId;

        // アイテムの詳細を設定
        UGCUpdateHandle_t update_handle =
            SteamUGC()->StartItemUpdate(434520, item_id);

        SteamUGC()->SetItemTitle(update_handle, "My Custom Pakset");
        SteamUGC()->SetItemDescription(update_handle, "Description...");
        SteamUGC()->SetItemTags(update_handle, {"pakset", "pak128"});
        SteamUGC()->SetItemContent(update_handle, "/path/to/my_pakset/");
        SteamUGC()->SetItemPreview(update_handle, "/path/to/preview.png");

        // アップロード実行
        SteamUGC()->SubmitItemUpdate(update_handle, "Initial release");
    }
}
```

---

## アップデートの管理

### バージョン管理

```cpp
void update_workshop_item(PublishedFileId_t item_id) {
    // アップデートを開始
    UGCUpdateHandle_t handle =
        SteamUGC()->StartItemUpdate(434520, item_id);

    // 変更されたファイルのみ更新
    SteamUGC()->SetItemContent(handle, "/path/to/updated_files/");

    // 変更履歴を記載
    SteamUGC()->SubmitItemUpdate(
        handle,
        "Version 1.1: Added new vehicles and fixed bugs"
    );
}
```

### 自動更新

Workshop アイテムは自動的に更新されます：

```cpp
void steam_t::on_item_downloaded(DownloadItemResult_t* callback) {
    // 新しいバージョンがダウンロードされた
    if (callback->m_eResult == k_EResultOK) {
        PublishedFileId_t item_id = callback->m_nPublishedFileId;

        // 古いバージョンをアンインストール
        uninstall_workshop_item(item_id);

        // 新しいバージョンをインストール
        install_workshop_item(item_id);

        // ゲームを再起動するよう促す
        show_restart_notification();
    }
}
```

---

## エラーハンドリング

### ダウンロードエラー

```cpp
void handle_download_error(EResult result) {
    std::string error_msg;

    switch (result) {
        case k_EResultFileNotFound:
            error_msg = "Workshop item not found";
            break;
        case k_EResultNotLoggedOn:
            error_msg = "Not logged into Steam";
            break;
        case k_EResultNoConnection:
            error_msg = "No internet connection";
            break;
        case k_EResultDiskFull:
            error_msg = "Not enough disk space";
            break;
        default:
            error_msg = "Unknown error";
    }

    // エラーメッセージを表示
    create_win(new error_dialog_t(error_msg.c_str()));
}
```

### インストールエラー

```cpp
bool workshop_item_t::install(std::string origin) {
    try {
        // ディレクトリが存在するか確認
        if (!directory_exists(origin)) {
            throw std::runtime_error("Source directory not found");
        }

        // インストール先が書き込み可能か確認
        if (!is_writable(env_t::user_dir)) {
            throw std::runtime_error("Destination not writable");
        }

        // ファイルをコピー
        copy_files(origin, get_install_path());

        return true;

    } catch (const std::exception& e) {
        dbg->error("workshop_item_t::install",
                   "Failed to install item %llu: %s",
                   id, e.what());
        return false;
    }
}
```

---

## 依存関係の管理

### Pakset の依存関係

一部のアドオンは特定の Pakset を必要とします：

```cpp
struct workshop_dependency_t {
    PublishedFileId_t required_item;
    std::string description;
};

class workshop_item_t {
private:
    std::vector<workshop_dependency_t> dependencies;

public:
    void add_dependency(PublishedFileId_t item_id, std::string desc) {
        dependencies.push_back({item_id, desc});
    }

    bool check_dependencies() {
        for (const auto& dep : dependencies) {
            if (!is_item_installed(dep.required_item)) {
                // 依存アイテムが未インストール
                return false;
            }
        }
        return true;
    }
};
```

### 自動的な依存関係の解決

```cpp
void install_with_dependencies(PublishedFileId_t item_id) {
    workshop_item_t item(item_id);

    // 依存関係をチェック
    if (!item.check_dependencies()) {
        // 必要なアイテムを購読
        for (const auto& dep : item.get_dependencies()) {
            SteamUGC()->SubscribeItem(dep.required_item);
        }
    }

    // アイテム自体をインストール
    item.install();
}
```

---

## パフォーマンスの考慮事項

### 非同期ダウンロード

Workshop アイテムのダウンロードは非同期で行われます：

```cpp
class workshop_download_manager_t {
private:
    std::queue<PublishedFileId_t> download_queue;
    bool is_downloading;

public:
    void queue_download(PublishedFileId_t item_id) {
        download_queue.push(item_id);
        process_queue();
    }

    void process_queue() {
        if (!is_downloading && !download_queue.empty()) {
            PublishedFileId_t next_item = download_queue.front();
            download_queue.pop();

            // Steam API でダウンロード開始
            SteamUGC()->DownloadItem(next_item, true);
            is_downloading = true;
        }
    }

    void on_download_complete() {
        is_downloading = false;
        process_queue();  // 次のアイテムを処理
    }
};
```

### キャッシュの活用

```cpp
class workshop_cache_t {
private:
    std::map<PublishedFileId_t, workshop_item_info_t> cache;

public:
    workshop_item_info_t* get_item_info(PublishedFileId_t item_id) {
        // キャッシュをチェック
        auto it = cache.find(item_id);
        if (it != cache.end()) {
            return &it->second;
        }

        // Steam API から取得
        workshop_item_info_t info = fetch_from_steam(item_id);
        cache[item_id] = info;
        return &cache[item_id];
    }
};
```

---

## セキュリティとサンドボックス

### ファイルの検証

```cpp
bool validate_workshop_item(const std::string& path) {
    // 1. ファイルサイズのチェック
    if (get_directory_size(path) > MAX_ITEM_SIZE) {
        return false;
    }

    // 2. 許可されていないファイルタイプをチェック
    if (contains_forbidden_files(path, {".exe", ".dll", ".so"})) {
        return false;
    }

    // 3. パス traversal 攻撃の防止
    if (contains_path_traversal(path)) {
        return false;
    }

    return true;
}
```

---

## デバッグとテスト

### Workshop テストモード

```cpp
#ifdef WORKSHOP_DEBUG
void enable_workshop_debug_mode() {
    // ローカルディレクトリから Workshop アイテムをシミュレート
    env_t::workshop_test_mode = true;
    env_t::workshop_test_path = "C:/dev/workshop_test/";
}
#endif
```

### ログ出力

```cpp
void workshop_item_t::install(std::string origin) {
    dbg->message("Workshop", "Installing item %llu from %s",
                 id, origin.c_str());

    for (const auto& file : files) {
        dbg->message("Workshop", "  Copying %s", file.c_str());
    }

    dbg->message("Workshop", "Installation complete");
}
```

---

## 関連ファイル

- **Workshop 管理**: `src/steam/workshop_item.{cc,h}`
- **Steam ラッパー**: `src/steam/steam.{cc,h}`
- **Pakset 管理**: `src/simutrans/dataobj/pakset_manager.{cc,h}`
- **ダウンロード UI**: `src/simutrans/gui/pakset_downloader_t.{cc,h}`

---

## 参考リンク

- [Steam Workshop ガイド（英語）](https://partner.steamgames.com/doc/features/workshop)
- [Steamworks API ドキュメント](https://partner.steamgames.com/doc/api/ISteamUGC)
- [Simutrans Steam Workshop](https://steamcommunity.com/app/434520/workshop/)

---

## まとめ

Steam Workshop 統合により、Simutrans のコンテンツ配布が大幅に簡素化されました。クリエイターは簡単に作品を公開でき、プレイヤーは簡単にインストールできます。自動更新、依存関係の管理、コミュニティ評価など、Steam のインフラを最大限に活用しています。

Workshop を活用して、Simutrans のコンテンツを作成・共有し、コミュニティを盛り上げましょう！
