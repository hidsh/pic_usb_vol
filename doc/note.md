# MPLABプロジェクトファイルの場所
ファイル:`pic_usb_vol/USB/Device - HID - Keyboard/Firmware/MPLAB.X`

# 主な処理内容
ファイル: `pic_usb_vol/USB/Device - HID - Keyboard/Firmware/Keyboard.c`

###### UserInit()
ロータリーエンコーダを読むためのポート設定を関数の後半に書いてある。

###### read_rotary_encoder()
ここでロータリーエンコーダを読む。 周期割り込み(?)からコールされる。

###### Keyboard()
ここでキーボードのレポートを送信する。 メインループからコールされる。

# ビルド環境
現在のビルド環境：`Mac OS 10.12 + MPLAB X + C18 Compiler`

- このプロジェクトで使われている古いUSBフレームワークはXC8ではコンパイルできず、C18でしかコンパイルできない。

- Unofficial ではあるが、MacOS 版の C18 Compiler を見つけた。これでMacでビルドができる。

Macでビルド実績のある MPLAB X と C18 Compiler のインストーラをenvフォルダに入れておく。

# PICkit3について
MPLAB X のインストールのときに MPLAB X IDEだけではなく MPLAB IPE も同時にインストールすること。 でないとMPLAB X が PICkit3 を認識できない。

MPLAB X が PICKIT3 を認識すると、プロジェクトのプロパティのデバッガ設定画面で PICkit3 のシリアル番号（例：BUR13xxxxなど）が表示される。

シリアル番号が表示されないときは認識されないので、この表示の有無で認識しているかどうかを判断する。

# PIC18F14K50 と PICkit3
実は PIC18F14K50 は ICE対応品と量産品の2種類がある。

通常市販されているPIC18F14K50はデバッグ支援回路が内蔵されていない量産品なので、PICKIT3をデバッガとして使うことはできない。つまりPICKIT3は書き込み器としてしか使えない。

PICKIT3をデバッガとして設定して、デバッグしようとすると

>Target Device ID (00004760) does not match expected Device ID (000047a0)

というメッセージが出るが、意訳するとこれは

>なんか、ICE対応品のじゃなくて量産品のデバイスIDが返ってきてんけど、量産品だとICEに対応してないからデバッグできひんよね。ほんまにこれでいいん？

的な意味になると思う。→ [参考リンク](https://www.microchip.co.jp/support/faq_8bit.html)

ちなみに量産品のPIC18F14K50でもAC244023というデバッグヘッダという基板を中継するとデバッグできるらしいが、この基板は50ドルもする模様。 なので、PIC18F14K50というデバイスはデバッガを使えないと割り切ったほうがいい。→ [参考リンク](http://www.microchipdirect.com/ProductSearch.aspx?keywords=AC244023)

# デバイスとデバッグ設定
PICKIT3がデバッガとして使えないので、プロジェクトのプロパティのデバイス設定の Supported Debug Header は Noneにしておく。

