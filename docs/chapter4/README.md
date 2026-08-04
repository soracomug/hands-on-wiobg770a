# 4: SORACOM へデータ送信して Harvest で確認

この章では、WioBG770a から SORACOM へデータを送信し、Harvest で受信結果を確認します.

## 想定時間

10 分

## この章のゴール

- WioBG770a  からセルラー通信でデータを送る
- Harvest Data 上で受信したデータを確認する
- 送信内容と表示内容の対応を理解する

## 事前に確認すること

この章では、前の章で設定した SIM グループを使います。次の状態になっていることを確認してください。

- WioBG770a に SIM が挿入されている
- SIM が所属するグループで SORACOM Harvest Data が ON になっている
- VSCode から WioBG770a にプログラムを実行できる

### セルラー接続する

### Harvest Data に送信する

## 実行結果を確認する

## Harvest Data で確認する

SORACOM ユーザーコンソールで `SIM 管理` を開き、対象の SIM にチェックを入れます。

![SIM 管理で対象の SIM を選択する](./images/console-sim-select.png)

`操作` をクリックし、メニューを下へスクロールして `ログと診断` の `Harvest Data を表示` をクリックします。

![操作メニューから Harvest Data を表示する](./images/console-show-harvest-data.png)

Harvest Data の画面が開きます。グラフ表示や表示範囲の変更など、詳しい画面操作は [Harvest Data のデータを確認する手順](https://users.soracom.io/ja-jp/docs/harvest/visualize/) を参照してください。

![Harvest Data で送信データを確認する](./images/console-harvest-data-result.png)

## FAQ

### Harvest Data にデータが表示されない

対象の SIM を選んでいるか確認してください。表示されない場合は、画面を再読み込みしてからもう一度確認します。

---
- 次: [5: 追加コンテンツ - センサーをつないでみる](../chapter5/README.md)
- 前: [3: SIM の開通と SORACOM Harvest Data の設定](../chapter3/README.md)
