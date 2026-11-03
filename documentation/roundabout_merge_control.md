# 2車線ラウンドアバウトの合流制御

issue #5「OTRPの2車線ラウンドアバウト向けに合流優先と進入制御を検討する」の実装メモ。

## 譲れ（合流優先）

交差点の1マス手前にある一方通行標識に「この先の交差点で譲る」を設定すると、
その標識から交差点へ入る車（バス・トラックと市内車）は、ほかの道から交差点へ近づく車がいる間は待つ。

- 待つ相手は、交差点につながるほかの道の上で、交差点へ向かって動いている車。
  止まっている車は待たない（環道が詰まっているときに互いに待ち合うのを避けるため）。
- 相手の出口が自分と違うとき、または相手の出口が分からないときは、必ず待つ。
- 出口が同じときは、車線が重なるときだけ待つ。
  - 右折（左側通行なら左折）で入る車は手前の車線にだけ入り、直進は両方の車線を横切るものとみなす。
  - 相手が車線変更中なら車線は重なるものとみなす。
- 見る範囲は交差点に隣り合う1マス。`debug.set_merge_yield_lookahead(n)` で3マスまで広げられる（比較用。ネットワークゲームでは変えられない）。
- 設定は標識の `choose_sign_flag` の1ビット（`merge_yield`）に入るので、セーブ形式は変わらない。
  変更は `TOOL_CHANGE_ROADSIGN` の `y` で行うため、ネットワークゲームでも同期する。

### 設定のしかた

- ゲーム内: 一方通行標識の情報ウィンドウで「この先の交差点で譲る」を押す。標識の次のマスが交差点のときだけ表示される。
- スクリプト: `command_x.set_merge_yield(player, pos, 1)`、確認は `tile.find_object(mo_roadsign).is_merge_yield()`。
- 自分の標識（または所有者のいない標識）だけ変えられる。公共プレイヤーはすべて変えられる。

## 計測

自動テストから使う計測用の関数を `debug` クラスに置いている。カウンタはセーブされず、ゲームの動きにも影響しない。

| 関数 | 内容 |
|---|---|
| `debug.set_merge_stats_enabled(bool)` | 計測の開始・停止 |
| `debug.reset_merge_stats()` | カウンタと車線の通過数を0に戻す |
| `debug.get_merge_stat(name)` | カウンタを読む。知らない名前なら -1 |
| `debug.watch_lane_passes(pos)` / `debug.get_lane_passes(pos, passing)` | マスに入った車を車線ごとに数える |

カウンタ名: `enter_checks` `enter_time_us` `citycar_checks` `citycar_time_us` `yield_checks` `yield_waits` `yield_time_us` `exit_checks` `exit_waits` `exit_time_us`

### 試験用の環道

`tests/automated-tests/tests/test_roundabout.nut` の `_ra_build()` が、4×4 の反時計回りの環道（x,y = 6〜9）と、
4方向の枝道（それぞれ折り返しループ付き）を作る。道はすべて一方通行の2車線（`oneway_mode`）。

ベンチマーク（`bench_roundabout_*`）は CI の対象外で、手元で次のように走らせる。

```
tests/run-automated-tests.sh bench_roundabout_uniform
```

`BENCH` で始まる行に、出口ごとの通過数、環道の車線ごとの通過数、流入ごとの最長待ち時間、計測カウンタが出る。
