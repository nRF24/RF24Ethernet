# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
<!-- markdownlint-disable MD024 -->

## [1.6.17] - 2024-10-06

### <!-- 1 --> 🚀 Added

- Add renewAddress if connect fails by \@TMRh20 in [`76a1511`](https://github.com/nRF24/RF24Ethernet/commit/76a15111457f58f0f78ab61326d2e694c526df43)
- Merge pull request \#49 from nRF24/mqtt_examples_renew by \@TMRh20 in [#49](https://github.com/nRF24/RF24Ethernet/pull/49)

### <!-- 3 --> 🗑️ Removed

- Example adjustments, new example by \@TMRh20 in [`9b78743`](https://github.com/nRF24/RF24Ethernet/commit/9b78743d0f9eb907a806678d92a13aa267a2c375)

### <!-- 9 --> 🗨️ Changed

- Update RF24Ethernet.h by \@TMRh20 in [`5b32020`](https://github.com/nRF24/RF24Ethernet/commit/5b32020e4b7dd6647c13f1ed0ac9df87ff28857a)
- [CI] add new example; rm PubSubClient dep by \@2bndy5 in [`a57a390`](https://github.com/nRF24/RF24Ethernet/commit/a57a390f9bd131086ac7637ea6509090ff40df8d)
- [CI] rm duplicated job in arduino workflow by \@2bndy5 in [`fd4ab23`](https://github.com/nRF24/RF24Ethernet/commit/fd4ab2396c63f8fb71414077063dd7d5acbb3611)
- Ignore unused parameters in new example by \@2bndy5 in [`a341c88`](https://github.com/nRF24/RF24Ethernet/commit/a341c8882569104e9f757c8c84f5e85d052e3106)
- Cast the deref'd value of unused parameter by \@2bndy5 in [`b0ae98a`](https://github.com/nRF24/RF24Ethernet/commit/b0ae98a83481a241c8e880a93c43670744d0c936)

[1.6.17]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.16...v1.6.17

Full commit diff: [`v1.6.16...v1.6.17`][1.6.17]

## [1.6.16] - 2024-07-01

### <!-- 1 --> 🚀 Added

- Merge pull request \#46 from nRF24/docs by \@TMRh20 in [#46](https://github.com/nRF24/RF24Ethernet/pull/46)
- Additions to last commit by \@TMRh20 in [`52722dd`](https://github.com/nRF24/RF24Ethernet/commit/52722dd338ce4e78e4dfbe9876b8a5cceba63896)

### <!-- 3 --> 🗑️ Removed

- Update RF24Ethernet config&setup by \@TMRh20 in [`6cf3278`](https://github.com/nRF24/RF24Ethernet/commit/6cf3278b30b3f48fcc80b61c59e7affb6850aa6f)
- Rexmit & hold by \@TMRh20 in [`ef264b6`](https://github.com/nRF24/RF24Ethernet/commit/ef264b67f3af27357a1a52b834e224a29cd509d8)

### <!-- 4 --> 🛠️ Fixed

- Merge pull request \#48 from nRF24/fixOutgoingData by \@TMRh20 in [#48](https://github.com/nRF24/RF24Ethernet/pull/48)

### <!-- 9 --> 🗨️ Changed

- Update examples with correct URL by \@TMRh20 in [`a49ef05`](https://github.com/nRF24/RF24Ethernet/commit/a49ef0564c04618dc54e80b8e091bfdb9488c8d1)
- Merge pull request \#47 from nRF24/FixURLAgain by \@TMRh20 in [#47](https://github.com/nRF24/RF24Ethernet/pull/47)
- Change YouTube link to newer video by \@TMRh20 in [`036480a`](https://github.com/nRF24/RF24Ethernet/commit/036480a572239db46bca717ef1cc4e69c599cd92)
- Need sudo for certain commands by \@TMRh20 in [`5254638`](https://github.com/nRF24/RF24Ethernet/commit/52546386bd66cf6b1fe717c6156d2c317800c0a5)
- Update Arduino setup info by \@TMRh20 in [`0fae40c`](https://github.com/nRF24/RF24Ethernet/commit/0fae40c10cbf713a56b25105f3c09432c0f54fae)
- Update doxygen CSS and use v1.11.0 by \@2bndy5 in [`ed31ad6`](https://github.com/nRF24/RF24Ethernet/commit/ed31ad698f4178e39974cf85c0222583e01479fb)
- Println to print by \@TMRh20 in [`0662744`](https://github.com/nRF24/RF24Ethernet/commit/0662744436ebbbea8015b73342b14b7e92983af4)
- Update RF24Ethernet.cpp by \@TMRh20 in [`5ea785a`](https://github.com/nRF24/RF24Ethernet/commit/5ea785af800fc9b66c95e59a456917e2f89a43c3)
- Merge branch 'fixOutgoingData' of https://github.com/nRF24/RF24Ethernet into fixOutgoingData by \@TMRh20 in [`1fd70e2`](https://github.com/nRF24/RF24Ethernet/commit/1fd70e27fcf2d54ef4eb8a5da9338e0eb2ab67fe)
- Partially revert prev change by \@TMRh20 in [`2c5ed33`](https://github.com/nRF24/RF24Ethernet/commit/2c5ed330629dcf3b691f7a2d8599016ead05ded9)
- Bump version to v1.6.16 by \@TMRh20 in [`4b9271a`](https://github.com/nRF24/RF24Ethernet/commit/4b9271a1f65ad6f8616cbaa3b02fc0dbebf41115)

[1.6.16]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.15...v1.6.16

Full commit diff: [`v1.6.15...v1.6.16`][1.6.16]

## [1.6.15] - 2024-06-11

### <!-- 4 --> 🛠️ Fixed

- Fix for BluePill by \@TMRh20 in [`c2e022b`](https://github.com/nRF24/RF24Ethernet/commit/c2e022b4b1cd76b88a6a5abdfa3410d0b47a55d5)
- Fix URL in client examples by \@TMRh20 in [`45fbb74`](https://github.com/nRF24/RF24Ethernet/commit/45fbb74e6cdf8b24f34ea11540f457a8c46fbbe9)

### <!-- 8 --> 📝 Documentation

- Minor doc updates by \@2bndy5 in [#45](https://github.com/nRF24/RF24Ethernet/pull/45)

### <!-- 9 --> 🗨️ Changed

- Minor change to last commit by \@TMRh20 in [`8af59af`](https://github.com/nRF24/RF24Ethernet/commit/8af59af132265541d9d6a780f7c3bf602f00413c)
- Merge pull request \#42 from nRF24/BluePillFix by \@TMRh20 in [#42](https://github.com/nRF24/RF24Ethernet/pull/42)
- Update clang format by \@2bndy5 in [#41](https://github.com/nRF24/RF24Ethernet/pull/41)
- Update examples/Getting_Started_SimpleClient_Mesh_DNS/Getting_Started_SimpleClient_Mesh_DNS.ino by \@TMRh20 in [`f32e377`](https://github.com/nRF24/RF24Ethernet/commit/f32e3777b9d8c5ad29e651204d0125b0b878142b)
- Merge pull request \#44 from nRF24/FixURL by \@TMRh20 in [#44](https://github.com/nRF24/RF24Ethernet/pull/44)
- Bump version to v1.6.15 by \@2bndy5 in [`590b307`](https://github.com/nRF24/RF24Ethernet/commit/590b307e25077c4a6705309213c77a19e7acdcfe)

[1.6.15]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.14...v1.6.15

Full commit diff: [`v1.6.14...v1.6.15`][1.6.15]

## [1.6.14] - 2023-10-26

### <!-- 1 --> 🚀 Added

- Add fix for ESP32 by \@TMRh20 in [`7fd48a7`](https://github.com/nRF24/RF24Ethernet/commit/7fd48a75ce222ecacc0bb6a96866ead48865fef3)

### <!-- 9 --> 🗨️ Changed

- Merge pull request \#38 from nRF24/ESP32-Fix by \@TMRh20 in [#38](https://github.com/nRF24/RF24Ethernet/pull/38)
- Simplify timeouts by \@TMRh20 in [`b92a75b`](https://github.com/nRF24/RF24Ethernet/commit/b92a75b0eb3d357b4143d88d312b3cab2db30b37)
- Minor update to latest changes by \@TMRh20 in [`844e849`](https://github.com/nRF24/RF24Ethernet/commit/844e8491b04644c3d1944044f04faa6440d00d50)
- Adjust for prev commit by \@TMRh20 in [`a1c353a`](https://github.com/nRF24/RF24Ethernet/commit/a1c353acabd2e7aedf53a4d24fad68eb7a9319d3)
- Clang format per \@2bndy5 \#39 by \@TMRh20 in [`778461e`](https://github.com/nRF24/RF24Ethernet/commit/778461ed25d5ed48b2880aeaf1c6c2fa42c9d75c)
- Merge pull request \#39 from nRF24/ConnectionTimeouts by \@TMRh20 in [#39](https://github.com/nRF24/RF24Ethernet/pull/39)
- Simplify connection timeouts more by \@TMRh20 in [`2e2b700`](https://github.com/nRF24/RF24Ethernet/commit/2e2b70045dcec9417e7511005b7db37aaa9bca93)
- Merge pull request \#40 from nRF24/ConnectionTimeouts by \@TMRh20 in [#40](https://github.com/nRF24/RF24Ethernet/pull/40)
- Update version for release by \@TMRh20 in [`368a8ef`](https://github.com/nRF24/RF24Ethernet/commit/368a8efdb5f53a053d1c2da7386d1ac5f362d92a)
- Ran clang-format on RF24Client.cpp by \@2bndy5 in [`52b9aae`](https://github.com/nRF24/RF24Ethernet/commit/52b9aaec0bffdf0166243d68bc9691b94c61e708)
- Modify triggers for PIO CI by \@2bndy5 in [`5996e13`](https://github.com/nRF24/RF24Ethernet/commit/5996e137842efb52832d8a2952319198f92163b1)

[1.6.14]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.13...v1.6.14

Full commit diff: [`v1.6.13...v1.6.14`][1.6.14]

## [1.6.13] - 2023-06-22

### <!-- 1 --> 🚀 Added

- Support NRF52x by \@TMRh20 in [`991fed3`](https://github.com/nRF24/RF24Ethernet/commit/991fed345ce428965abfa3549bbe7d81148fdb72)

### <!-- 9 --> 🗨️ Changed

- Update version for release by \@TMRh20 in [`0d0e019`](https://github.com/nRF24/RF24Ethernet/commit/0d0e019f20d3b635722c7fbb4c058a583b70ff70)
- Update for nrf52 by \@TMRh20 in [`917cf67`](https://github.com/nRF24/RF24Ethernet/commit/917cf673d698522494c8de994df9f08cf8d433d1)
- Changes for network/mesh v2.0 by \@TMRh20 in [`f41b8ec`](https://github.com/nRF24/RF24Ethernet/commit/f41b8ece5d61696959496d38a25516e450f5a4d3)
- Update per review by \@TMRh20 in [`f6c5460`](https://github.com/nRF24/RF24Ethernet/commit/f6c546070adfb04ebea4d958a90bebc393865fdc)
- Update README.md by \@TMRh20 in [`d560cb2`](https://github.com/nRF24/RF24Ethernet/commit/d560cb20036b1a83d82ad526ab6fdefa7505fd29)
- Merge pull request \#37 from nRF24/NRF52-support by \@TMRh20 in [#37](https://github.com/nRF24/RF24Ethernet/pull/37)
- Merge branch 'master' of https://github.com/nRF24/RF24Ethernet by \@TMRh20 in [`d3ded1b`](https://github.com/nRF24/RF24Ethernet/commit/d3ded1bf6ce31b94706cb88d55d5dfefe9d5c30d)

[1.6.13]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.12...v1.6.13

Full commit diff: [`v1.6.12...v1.6.13`][1.6.13]

## [1.6.12] - 2023-06-06

### <!-- 1 --> 🚀 Added

- Add link to Arduino Project Hub tutorial by \@TMRh20 in [`1c143ae`](https://github.com/nRF24/RF24Ethernet/commit/1c143ae81c22da5d6fbd0acd7a449a13ac897cb4)
- Add `deploy-release` arg to PIO reusable CI call by \@2bndy5 in [`1f651f6`](https://github.com/nRF24/RF24Ethernet/commit/1f651f6cd0d78b831ad95d2e7b9770c81f5c4520)

### <!-- 4 --> 🛠️ Fixed

- Fix for RPi Pico by \@TMRh20 in [`d661558`](https://github.com/nRF24/RF24Ethernet/commit/d6615583c1a19efe9cf89e2aa8aaa0e80b0e1ab6)

### <!-- 8 --> 📝 Documentation

- Manually trigger docs CI by \@2bndy5 in [`0649526`](https://github.com/nRF24/RF24Ethernet/commit/0649526c311255ec34ee0e5666df65c5a990e9d8)
- Doc updates and reusable CI by \@2bndy5 in [`0f3f6d9`](https://github.com/nRF24/RF24Ethernet/commit/0f3f6d901967ce518ed283cfc977b5f7d0b813bb)

### <!-- 9 --> 🗨️ Changed

- Update extra lib deps for PIO CI by \@2bndy5 in [`440ddad`](https://github.com/nRF24/RF24Ethernet/commit/440ddad95288e8ed39fc570d12ec26398d8c258c)
- Update version for release by \@TMRh20 in [`0f5af50`](https://github.com/nRF24/RF24Ethernet/commit/0f5af50cae944a9e05bdd9ffa2f432cfb57254c4)

[1.6.12]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.11...v1.6.12

Full commit diff: [`v1.6.11...v1.6.12`][1.6.12]

## [1.6.11] - 2022-07-21

### <!-- 4 --> 🛠️ Fixed

- Fix HTTP header responses by \@TMRh20 in [`acc187e`](https://github.com/nRF24/RF24Ethernet/commit/acc187e9c86a60f02e3281e26eb2a8a46bb6de03)
- Merge pull request \#35 from nRF24/Fix-HTTP-headers by \@TMRh20 in [#35](https://github.com/nRF24/RF24Ethernet/pull/35)

### <!-- 9 --> 🗨️ Changed

- Clang Formatting by \@TMRh20 in [`9954db3`](https://github.com/nRF24/RF24Ethernet/commit/9954db36feb1d40e29404ed438565af0c026e3ef)
- Minor version bump by \@TMRh20 in [`50e3f2a`](https://github.com/nRF24/RF24Ethernet/commit/50e3f2a31c83c0e107167d449b913e1d75227101)

[1.6.11]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.10...v1.6.11

Full commit diff: [`v1.6.10...v1.6.11`][1.6.11]

## [1.6.10] - 2022-07-09

### <!-- 1 --> 🚀 Added

- Add bluepill board to PIO CI by \@2bndy5 in [`95477fe`](https://github.com/nRF24/RF24Ethernet/commit/95477fe9d2b0b1cf6e4b4e3738112523f25c1fd4)
- Add clang-format config by \@2bndy5 in [`8d70eb0`](https://github.com/nRF24/RF24Ethernet/commit/8d70eb05129f1786f490389f88bc373b8c250e8c)

### <!-- 3 --> 🗑️ Removed

- Remove old AStyle config by \@2bndy5 in [`64d7715`](https://github.com/nRF24/RF24Ethernet/commit/64d7715eec7858ec979124e1120cc5210462fba9)

### <!-- 4 --> 🛠️ Fixed

- Fix error in examples by \@TMRh20 in [`e6bd624`](https://github.com/nRF24/RF24Ethernet/commit/e6bd624f1bb4d6c648f8a6bf41ae63fa1789f27d)
- Fix for \#28 by \@TMRh20 in [`03bead3`](https://github.com/nRF24/RF24Ethernet/commit/03bead354c6bca050c56dd20cad01b4a9542ec9f)
- Fix connection issues by \@TMRh20 in [#33](https://github.com/nRF24/RF24Ethernet/pull/33)

### <!-- 9 --> 🗨️ Changed

- Update clock-arch.c by \@TMRh20 in [`88820be`](https://github.com/nRF24/RF24Ethernet/commit/88820be1b54033987b60aaea7ecb13d42b86c25f)
- Update licenses for PIO lib registry by \@2bndy5 in [`8506d9c`](https://github.com/nRF24/RF24Ethernet/commit/8506d9cfff9721534ad788b91602ef0b68319c67)
- Merge pull request \#29 from nRF24/Fix\#28 by \@TMRh20 in [#29](https://github.com/nRF24/RF24Ethernet/pull/29)
- Update Arduino CI workflow by \@2bndy5 in [`2ccc1ed`](https://github.com/nRF24/RF24Ethernet/commit/2ccc1eddcfddc2349e5800124ba39330298b3d8c)
- There is no testing for ATTiny here by \@2bndy5 in [`96c3e0b`](https://github.com/nRF24/RF24Ethernet/commit/96c3e0b4a5512e5a0dd219ef863ecf322b522da0)
- Merge pull request \#32 from nRF24/enable-code-size-delta-reports by \@TMRh20 in [#32](https://github.com/nRF24/RF24Ethernet/pull/32)
- Update CI workflows by \@2bndy5 in [`2c22d90`](https://github.com/nRF24/RF24Ethernet/commit/2c22d90b9d7d5ce1d48edfdef84c83b1fa0b8405)
- Ran clang-format on all relevant sources by \@2bndy5 in [`a4d4834`](https://github.com/nRF24/RF24Ethernet/commit/a4d483442e34a3520dca3b40677b4c24400502a5)
- Oops! use actual arduino config for examples by \@2bndy5 in [`d660bd6`](https://github.com/nRF24/RF24Ethernet/commit/d660bd60d3781c350cdb891558adf1eb24824bf4)
- Merge pull request \#34 from nRF24/clang-format by \@TMRh20 in [#34](https://github.com/nRF24/RF24Ethernet/pull/34)
- Minor version bump by \@2bndy5 in [`726eb0d`](https://github.com/nRF24/RF24Ethernet/commit/726eb0da94eb041164914a378b0ef273eb10777b)

[1.6.10]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.9...v1.6.10

Full commit diff: [`v1.6.9...v1.6.10`][1.6.10]

## [1.6.9] - 2021-11-28

### <!-- 1 --> 🚀 Added

- Add lib.json and remove some examples from CI by \@2bndy5 in [`106e6ad`](https://github.com/nRF24/RF24Ethernet/commit/106e6adaae8d75d69d3d331ab8c8b369547dd95a)
- Add a better example by \@TMRh20 in [`1b5c1e5`](https://github.com/nRF24/RF24Ethernet/commit/1b5c1e51ed5d03bc3fa11462d601b1d72c7f193f)
- Add a retry to network_send() by \@TMRh20 in [`98381b7`](https://github.com/nRF24/RF24Ethernet/commit/98381b78b394cd6301392665a2b95f98eb7a89b1)
- Add even better example by \@TMRh20 in [`5a60e61`](https://github.com/nRF24/RF24Ethernet/commit/5a60e611ee2261ae28b5efd0bbe9a2076fff7969)
- Add DNS/UDP example by \@TMRh20 in [`7aef919`](https://github.com/nRF24/RF24Ethernet/commit/7aef919a9acaef7357e1a88eb519fda2447799a0)

### <!-- 3 --> 🗑️ Removed

- Remove unnecessary memory buffer by \@TMRh20 in [`d96617a`](https://github.com/nRF24/RF24Ethernet/commit/d96617ae084b1737fcc4f35192c4e3da7c92e70d)

### <!-- 4 --> 🛠️ Fixed

- Fix broken link in docs by \@2bndy5 in [`7e74375`](https://github.com/nRF24/RF24Ethernet/commit/7e743757968b819388be4b9e1d503c18e6d4d4d9)
- Fix formatting in last commit by \@2bndy5 in [`ed7232d`](https://github.com/nRF24/RF24Ethernet/commit/ed7232d9a730eda058a62f8bb1f36b5c09551397)
- Fix formatting by \@TMRh20 in [`329ff2b`](https://github.com/nRF24/RF24Ethernet/commit/329ff2b3937e44bf4978ccf268dff42404cb8e1f)
- Fix error in example by \@TMRh20 in [`16d0a2c`](https://github.com/nRF24/RF24Ethernet/commit/16d0a2cf56820a4a765ebe526bcabf2199b38be8)

### <!-- 8 --> 📝 Documentation

- Verify docs locally by \@2bndy5 in [`0a4ab3e`](https://github.com/nRF24/RF24Ethernet/commit/0a4ab3eee58d44c09db3e57bdbc8590a53ce1fad)

### <!-- 9 --> 🗨️ Changed

- Use memcpy instead of byte packing. Makes it compatible with STM32 based boards in [`f565c79`](https://github.com/nRF24/RF24Ethernet/commit/f565c79f2fe7aa98f6409569de083604ff170038)
- Merge pull request \#25 from microtronics/stm32fix by \@TMRh20 in [#25](https://github.com/nRF24/RF24Ethernet/pull/25)
- Fmt examples & add CI for arduino and PIO by \@2bndy5 in [`518d5fb`](https://github.com/nRF24/RF24Ethernet/commit/518d5fb54fee433d67bc99bfcbb19e4b322241ff)
- Used AStyle.exe correctly this time by \@2bndy5 in [`8094b9c`](https://github.com/nRF24/RF24Ethernet/commit/8094b9c31060b5dbb5eed2712aaf1bf568bd61b4)
- [PIO CI] stm32 core seems broken by \@2bndy5 in [`4e4fb10`](https://github.com/nRF24/RF24Ethernet/commit/4e4fb10615d86e5bd3060c2ced2cf503c9c56e7e)
- [RF24Server on esp32] add overload begin(uint16_t) by \@2bndy5 in [`361d3ec`](https://github.com/nRF24/RF24Ethernet/commit/361d3ec9e7ef54937e2691f191fdd3e924178cb4)
- Gimme em badges! glob multi-file example in PIO CI by \@2bndy5 in [`cba39de`](https://github.com/nRF24/RF24Ethernet/commit/cba39de0df674785e054fe718bb8339a7f92a710)
- Update doxygen CI workflow by \@2bndy5 in [`9cdb734`](https://github.com/nRF24/RF24Ethernet/commit/9cdb73459c30ed6f1ec7d1ed7eaab50ad75117d2)
- [PIO CI] rename saved artifact by \@2bndy5 in [`cc1f554`](https://github.com/nRF24/RF24Ethernet/commit/cc1f5540a7e1be7c68459934750ad1ee7dafaf7e)
- [PIO CI] re-rename saved artifact by \@2bndy5 in [`be4ce32`](https://github.com/nRF24/RF24Ethernet/commit/be4ce32c215651764dc8395821610ffc6b0e16e8)
- Merge pull request \#26 from nRF24/add-ci by \@TMRh20 in [#26](https://github.com/nRF24/RF24Ethernet/pull/26)
- Doxygen does like md hyperlinks in section titles by \@2bndy5 in [`e861b92`](https://github.com/nRF24/RF24Ethernet/commit/e861b92bfcbf6214d0b9e8ad239bd2bb121cf7b2)
- Auto-publish to PIO by \@2bndy5 in [`ff9acc9`](https://github.com/nRF24/RF24Ethernet/commit/ff9acc9b38c780369e95da650c40678fc70287cf)
- Use --non-interactive option in PIO CI by \@2bndy5 in [`1dd910d`](https://github.com/nRF24/RF24Ethernet/commit/1dd910d9a2871161ae8b5ea3a11548d90a19b09d)
- Update README.md by \@TMRh20 in [`f36b353`](https://github.com/nRF24/RF24Ethernet/commit/f36b353a79aada472f9eada7b856ff6cc7799781)
- Update example by \@TMRh20 in [`1863140`](https://github.com/nRF24/RF24Ethernet/commit/1863140d7a2e45324af2c8d23bf554b5652a2ef2)
- Update example by \@TMRh20 in [`58be122`](https://github.com/nRF24/RF24Ethernet/commit/58be122fb543f8f2ee44436685f2eff73f1a4b57)
- Replace pubsub example by \@TMRh20 in [`024d455`](https://github.com/nRF24/RF24Ethernet/commit/024d455169df8a13a14fe3e5054df6a076f532c2)
- Consistent mesh management by \@TMRh20 in [`44e456b`](https://github.com/nRF24/RF24Ethernet/commit/44e456b726b5eccb1213852c50cbf1df3569d75c)
- Update Getting_Started_SimpleServer_Mesh.ino by \@TMRh20 in [`db724a7`](https://github.com/nRF24/RF24Ethernet/commit/db724a724366bf0a2effc1648aca1b7fea86749d)
- Clean up uip_userdata_t struct by \@TMRh20 in [`a9936c8`](https://github.com/nRF24/RF24Ethernet/commit/a9936c83a9a8f894d2236db1ff11fe6d95d4f83a)
- Update Getting_Started_SimpleClient_Mesh_DNS.ino by \@TMRh20 in [`49440fa`](https://github.com/nRF24/RF24Ethernet/commit/49440fa611af03f168529e25e039e89c6ef71efa)
- Correct default IPs by \@TMRh20 in [`8a4028b`](https://github.com/nRF24/RF24Ethernet/commit/8a4028b4443e084a73c15ba79d4035a797dad83e)
- Update library.properties by \@TMRh20 in [`d889b28`](https://github.com/nRF24/RF24Ethernet/commit/d889b2827820ee6df8aa7818cbbd7107c02469e4)
- Update library.json by \@TMRh20 in [`229e7c3`](https://github.com/nRF24/RF24Ethernet/commit/229e7c330b7a09712204d9d125c37b66f4c76ae5)

[1.6.9]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.8...v1.6.9

Full commit diff: [`v1.6.8...v1.6.9`][1.6.9]

## New Contributors
* \@ made their first contribution
## [1.6.8] - 2021-04-13

### <!-- 8 --> 📝 Documentation

- Docs abstraction & code formating by \@2bndy5 in [#22](https://github.com/nRF24/RF24Ethernet/pull/22)

### <!-- 9 --> 🗨️ Changed

- Version bump (patch+1) by \@2bndy5 in [`4e0a01e`](https://github.com/nRF24/RF24Ethernet/commit/4e0a01e1922ed7ec555ab873102f60f18df75387)

[1.6.8]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.7...v1.6.8

Full commit diff: [`v1.6.7...v1.6.8`][1.6.8]

## [1.6.7] - 2020-12-14

### <!-- 4 --> 🛠️ Fixed

- Fix warnings; show files list; ref HTML.h properly by \@2bndy5 in [`bb0b29d`](https://github.com/nRF24/RF24Ethernet/commit/bb0b29dc74bad9e9f37ed004e14b3feeb06a671e)
- Fix missing images by \@2bndy5 in [`ad0980c`](https://github.com/nRF24/RF24Ethernet/commit/ad0980c69f7918e73a4c56e480efaf7622644898)

### <!-- 8 --> 📝 Documentation

- Redirect doc links to nRF24 org by \@2bndy5 in [`ff4a678`](https://github.com/nRF24/RF24Ethernet/commit/ff4a678676cc2ad07eda0da8efc1c9b33cbca103)
- Merge pull request \#20 from 2bndy5/master by \@TMRh20 in [#20](https://github.com/nRF24/RF24Ethernet/pull/20)

### <!-- 9 --> 🗨️ Changed

- Edit doxyfile OUTPUT_DIR & add workflow by \@2bndy5 in [`6e11fe7`](https://github.com/nRF24/RF24Ethernet/commit/6e11fe7fce5cebf6c79574b385189de0764903b3)
- Format doxygen css file & include latest changes by \@2bndy5 in [`e05b8ac`](https://github.com/nRF24/RF24Ethernet/commit/e05b8acd44f41ab7f24f3229bddddedd31585dfd)
- Only run doxygen on master branch by \@2bndy5 in [`4b92ed4`](https://github.com/nRF24/RF24Ethernet/commit/4b92ed43b22d899e33c999a6337f381459148336)
- Del "_Dev" part of dead links by \@2bndy5 in [`ef0e46f`](https://github.com/nRF24/RF24Ethernet/commit/ef0e46f709d222240c915f6da4b0dbae2a986d72)
- Merge pull request \#21 from 2bndy5/master by \@2bndy5 in [#21](https://github.com/nRF24/RF24Ethernet/pull/21)
- Update library.properties by \@TMRh20 in [`ef5b331`](https://github.com/nRF24/RF24Ethernet/commit/ef5b3313ba62e7c48aad04a98bfd0d9d419ed9bd)

[1.6.7]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.6...v1.6.7

Full commit diff: [`v1.6.6...v1.6.7`][1.6.7]

## [1.6.6] - 2020-11-18

### <!-- 1 --> 🚀 Added

- Add alternate MQTT example by \@TMRh20 in [`3910456`](https://github.com/nRF24/RF24Ethernet/commit/3910456d0e9e5e52b065a610fb8b2033ce24de72)

### <!-- 9 --> 🗨️ Changed

- Update example in header file by \@TMRh20 in [`535ed65`](https://github.com/nRF24/RF24Ethernet/commit/535ed65a3a30132ba3b3d81575821cd9a0a71004)
- Update library.properties by \@TMRh20 in [`346aca3`](https://github.com/nRF24/RF24Ethernet/commit/346aca3db6edee285f1fed82f62b395fd59bccdb)

[1.6.6]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.5...v1.6.6

Full commit diff: [`v1.6.5...v1.6.6`][1.6.6]

## [1.6.5] - 2020-08-17

### <!-- 9 --> 🗨️ Changed

- Update README.md by \@TMRh20 in [`329fec9`](https://github.com/nRF24/RF24Ethernet/commit/329fec9ec08c31bb0e63efbcf1c9943968fb9162)
- Bug fixes for memory issues etc by \@TMRh20 in [`51c3a98`](https://github.com/nRF24/RF24Ethernet/commit/51c3a989aa4a1b351d3b3a66b0a4789034e4fa0a)
- SimpleClient Example: pizza & a book by \@TMRh20 in [`564d94e`](https://github.com/nRF24/RF24Ethernet/commit/564d94e9f4523033d02a659b43efde3ab363907c)
- Update for release by \@TMRh20 in [`f329f33`](https://github.com/nRF24/RF24Ethernet/commit/f329f33dad37184ff4a7278739106308aeac4329)

[1.6.5]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.4...v1.6.5

Full commit diff: [`v1.6.4...v1.6.5`][1.6.5]

## [1.6.4] - 2020-08-01

### <!-- 3 --> 🗑️ Removed

- Fix for MQTT example by \@TMRh20 in [`09ff6d3`](https://github.com/nRF24/RF24Ethernet/commit/09ff6d32e9b3bb7913f326b6a08a2df50c3d8aaf)
- Clean up by \@TMRh20 in [`a58c42c`](https://github.com/nRF24/RF24Ethernet/commit/a58c42cdf195368dc0eac1dc0cf54f77acf8bd60)
- Clean up by \@TMRh20 in [`7d23534`](https://github.com/nRF24/RF24Ethernet/commit/7d23534c93cdd47a1eb9d40c034f204b452b882e)

### <!-- 4 --> 🛠️ Fixed

- Fix bugginess on AVR devices by \@TMRh20 in [`752772d`](https://github.com/nRF24/RF24Ethernet/commit/752772dd634c1e3364152d95f0356b2e6da35bd4)

### <!-- 9 --> 🗨️ Changed

- Minor fix for MQTT example by \@TMRh20 in [`30ccf24`](https://github.com/nRF24/RF24Ethernet/commit/30ccf24427c2f2c5ac71197ca86b6418d51a7aff)
- Cleanup warnings/minor issues by \@TMRh20 in [`946f00b`](https://github.com/nRF24/RF24Ethernet/commit/946f00b25fbe6aa31866ed47e28602bb321a9081)
- Update version for release by \@TMRh20 in [`d57eeef`](https://github.com/nRF24/RF24Ethernet/commit/d57eeef40543e029e9b9cc6830661179d1e316e0)

[1.6.4]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.3...v1.6.4

Full commit diff: [`v1.6.3...v1.6.4`][1.6.4]

## [1.6.3] - 2020-04-08

### <!-- 9 --> 🗨️ Changed

- Update MQTT example by \@TMRh20 in [`e444a3c`](https://github.com/nRF24/RF24Ethernet/commit/e444a3c24b75a55c016c2d553b4bd5285d695a46)
- Improve responsiveness of mqtt sketch by \@TMRh20 in [`0905344`](https://github.com/nRF24/RF24Ethernet/commit/09053442995373cf4102e396033e2e9cb6a2fa38)
- Specify library dependencies in library.properties by \@per1234 in [`603b15b`](https://github.com/nRF24/RF24Ethernet/commit/603b15b9586119efeb7e82220e274f26a4581f0d)
- Merge pull request \#18 from per1234/depends by \@TMRh20 in [#18](https://github.com/nRF24/RF24Ethernet/pull/18)
- Update version for current release by \@TMRh20 in [`83d778c`](https://github.com/nRF24/RF24Ethernet/commit/83d778c96b2692572b9393378aed72c038e04d84)

[1.6.3]: https://github.com/nRF24/RF24Ethernet/compare/v1.6.2...v1.6.3

Full commit diff: [`v1.6.2...v1.6.3`][1.6.3]

## New Contributors
* \@per1234 made their first contribution
## [1.6.2] - 2019-05-09

### <!-- 4 --> 🛠️ Fixed

- Fix for non-AVR devices by \@TMRh20 in [`fc55fad`](https://github.com/nRF24/RF24Ethernet/commit/fc55fad1c59c873518407a25d209f4fefaa2f674)

### <!-- 9 --> 🗨️ Changed

- Attempt compliance with GPL per lucky \#13 by \@TMRh20 in [`9376e45`](https://github.com/nRF24/RF24Ethernet/commit/9376e451b657ba25a0be798686f31f0570e1b3bc)
- Revert window size change, MQTT: Unique client ID by \@TMRh20 in [`e3bca34`](https://github.com/nRF24/RF24Ethernet/commit/e3bca342b2ab1e0432ef2c2907b35c3fb9c55b2d)
- Correct url in library.properties by \@TMRh20 in [`39f9ced`](https://github.com/nRF24/RF24Ethernet/commit/39f9ced23966d5025e4c0a8429c15a5d80794fb4)
- Update version for release by \@TMRh20 in [`f284197`](https://github.com/nRF24/RF24Ethernet/commit/f2841976f220f26cb90955643eb49aa621a52dac)

[1.6.2]: https://github.com/nRF24/RF24Ethernet/compare/1.6.1...v1.6.2

Full commit diff: [`1.6.1...v1.6.2`][1.6.2]

## [1.6.1] - 2015-12-18

### <!-- 3 --> 🗑️ Removed

- Slight reversion of prev changes by \@TMRh20 in [`2cf04ed`](https://github.com/nRF24/RF24Ethernet/commit/2cf04edcef55826ad6bd615e90a0a5f95ba7ae03)

### <!-- 4 --> 🛠️ Fixed

- Fix client operator conversion per \@kluzzebass \#10 by \@TMRh20 in [`3b1bb22`](https://github.com/nRF24/RF24Ethernet/commit/3b1bb22de21bda3114f0779ef3d7c1b2238ee0db)
- Fix mqtt example by \@TMRh20 in [`01873b0`](https://github.com/nRF24/RF24Ethernet/commit/01873b0e26b874217f738a8fb76763ee92eb988d)

### <!-- 8 --> 📝 Documentation

- Connection retries, Arduino yield() function by \@TMRh20 in [`9afea4f`](https://github.com/nRF24/RF24Ethernet/commit/9afea4fd9095faed1193eff169ea3d95420a0c18)

### <!-- 9 --> 🗨️ Changed

- Workaround for yield() crash on ESP8266 by \@TMRh20 in [`70315fb`](https://github.com/nRF24/RF24Ethernet/commit/70315fbb71acdae63ccc64f8edda565fb4af4a91)
- Update/fix SLIP gateway example by \@TMRh20 in [`ecbee0a`](https://github.com/nRF24/RF24Ethernet/commit/ecbee0aa486a517824b4b89e8b192e4d1e45cf30)
- Update library properties by \@TMRh20 in [`97e6b4c`](https://github.com/nRF24/RF24Ethernet/commit/97e6b4c69ef61b74831137bf7ba5fb399f21eb0d)

[1.6.1]: https://github.com/nRF24/RF24Ethernet/compare/1.6...1.6.1

Full commit diff: [`1.6...1.6.1`][1.6.1]

## [1.6] - 2015-11-29

### <!-- 1 --> 🚀 Added

- Add readme by \@TMRh20 in [`65066ec`](https://github.com/nRF24/RF24Ethernet/commit/65066ec67abaca58cd1cf3a4bf2341cacdc46426)
- Add multicast relay, modify TCP window handling by \@TMRh20 in [`d822d3e`](https://github.com/nRF24/RF24Ethernet/commit/d822d3e79b51c984fa65f891715533b692067b6e)
- Add documentation link to readme by \@TMRh20 in [`7ee89d4`](https://github.com/nRF24/RF24Ethernet/commit/7ee89d425b29552604ade02d5208c12c1a24318a)
- Add example using RF24Mesh for address assignment by \@TMRh20 in [`c829c16`](https://github.com/nRF24/RF24Ethernet/commit/c829c164c1ce3f1c5e6129ea4d963e3ee6bdf55e)
- Add missing timer files by \@TMRh20 in [`be53c75`](https://github.com/nRF24/RF24Ethernet/commit/be53c75679c90ae6407dc98e8c7d869d00ee7cf8)
- Add waitAvailable() , reduce memory by \@TMRh20 in [`0f3b917`](https://github.com/nRF24/RF24Ethernet/commit/0f3b917cac3926dd5588964f8d153034ebb3d255)
- Add Ethernet.update() function by \@TMRh20 in [`fead31f`](https://github.com/nRF24/RF24Ethernet/commit/fead31f118a5c213a9bfd34eefccbb33a7ab4134)
- Add address renewal to client example by \@TMRh20 in [`62a244d`](https://github.com/nRF24/RF24Ethernet/commit/62a244d85852dd790c90b044fca1ad21ca21775b)
- Add info to readme per \#3 by \@TMRh20 in [`bd1fc76`](https://github.com/nRF24/RF24Ethernet/commit/bd1fc76e2686f67d0b2bb6cff69f7ae736dc50fa)
- Add ESP8266 example with dual interfaces by \@TMRh20 in [`834da23`](https://github.com/nRF24/RF24Ethernet/commit/834da2396734bb72791e1fe784df9a87dc224ccc)
- Add library.properties by \@TMRh20 in [`427f0e7`](https://github.com/nRF24/RF24Ethernet/commit/427f0e7aac30ee4b4d0db609a941d3980efd6fba)

### <!-- 3 --> 🗑️ Removed

- Major update v1.2b, add UDP, DNS support by \@TMRh20 in [`c7c21d3`](https://github.com/nRF24/RF24Ethernet/commit/c7c21d3898cb62df301806b83c8f3debbf624b06)
- Cleanup for main client and server code by \@TMRh20 in [`7455ab3`](https://github.com/nRF24/RF24Ethernet/commit/7455ab3daa8b3389ecd00f67b8a170e3b65744d8)
- V1.5b - Update for RF24Gateway/RF24Mesh defaults by \@TMRh20 in [`e4d108a`](https://github.com/nRF24/RF24Ethernet/commit/e4d108a219d8b79799842c760f6cf76f37a2a99b)

### <!-- 4 --> 🛠️ Fixed

- Fix readme by \@TMRh20 in [`146eeff`](https://github.com/nRF24/RF24Ethernet/commit/146eeffc82b1fffff0485d4eff2e84c2eff85398)
- Fix instructions for SLIP_Gateway example by \@TMRh20 in [`e8305b8`](https://github.com/nRF24/RF24Ethernet/commit/e8305b84d275710132e0b82ef663312952601800)
- Prevent stalling during downloads by \@TMRh20 in [`bc32545`](https://github.com/nRF24/RF24Ethernet/commit/bc32545740f9568e01ab05b84d53654f9dd38132)
- Corrupt client requests, reliability by \@TMRh20 in [`0d698e4`](https://github.com/nRF24/RF24Ethernet/commit/0d698e442da257cf97bb5f796cd2f97af2850e49)
- Hang on failed writes, Debugging by \@TMRh20 in [`1efcca8`](https://github.com/nRF24/RF24Ethernet/commit/1efcca87b8e714e65142a0ef9e098dbe6d635192)
- Fix server include for last change by \@TMRh20 in [`4120ba2`](https://github.com/nRF24/RF24Ethernet/commit/4120ba21d760b964e16c6465fdafd223c983267b)
- UDP/DNS with TUN/RF24Mesh by \@TMRh20 in [`a393f97`](https://github.com/nRF24/RF24Ethernet/commit/a393f9777fe352706b08855710415a277d9eb4f2)
- Fix compile in Arduino IDE 1.0.6 by \@TMRh20 in [`a92d9ff`](https://github.com/nRF24/RF24Ethernet/commit/a92d9ff99cd030b34ea3348ad157c195452c5845)
- Fix mistake in change to exmaples by \@TMRh20 in [`e185890`](https://github.com/nRF24/RF24Ethernet/commit/e1858903a345ce2e29380eb49cd98096bbb5a750)
- Fixes for outgoing data handling by \@TMRh20 in [`e852e1d`](https://github.com/nRF24/RF24Ethernet/commit/e852e1d723d49da3d8c24e55afa01ce7980324ea)
- Hanging on writes by \@TMRh20 in [`3049d3a`](https://github.com/nRF24/RF24Ethernet/commit/3049d3a11040f2d1532e67e5d054423b60de4e33)
- Fix HTML.h broken on last update (Server examples) by \@TMRh20 in [`2994669`](https://github.com/nRF24/RF24Ethernet/commit/299466902a088c6a98477b405147dc18f73c32c1)
- Fix bug, playing w/RF24 IP interface on ESP8266 by \@TMRh20 in [`84e87d3`](https://github.com/nRF24/RF24Ethernet/commit/84e87d38f96cfa0590fa63d4697148182205dd90)

### <!-- 9 --> 🗨️ Changed

- :circus_tent: Added .gitattributes & .gitignore files by \@TMRh20 in [`eb9a4bd`](https://github.com/nRF24/RF24Ethernet/commit/eb9a4bd7629afbcfa46f5e481330dfda1f5cd1d8)
- Chg readme to point to current development fork by \@TMRh20 in [`1a50c46`](https://github.com/nRF24/RF24Ethernet/commit/1a50c46097f19a6e67b2f77fd9a17d43b2da7452)
- Check in current code of RF24Ethernet by \@TMRh20 in [`3a7b37e`](https://github.com/nRF24/RF24Ethernet/commit/3a7b37ed2f0b14178836cddcea5552b80aede601)
- Check-in todays updates by \@TMRh20 in [`74fe127`](https://github.com/nRF24/RF24Ethernet/commit/74fe127b07b658fb380f71a74aa90da91f0612cd)
- Update readme by \@TMRh20 in [`e305a42`](https://github.com/nRF24/RF24Ethernet/commit/e305a4290de6ff6990a6082532ac25dcb1f06e9b)
- Check in current code by \@TMRh20 in [`6f5f7c5`](https://github.com/nRF24/RF24Ethernet/commit/6f5f7c58143436e6416172a91ab180f5f59cf1cf)
- Bring more in-line with Arduino Ethernet API by \@TMRh20 in [`497d8a9`](https://github.com/nRF24/RF24Ethernet/commit/497d8a98d72c94b7e4b2f636b3cf883cf8c4f09a)
- Clean up examples by \@TMRh20 in [`8c6b3a2`](https://github.com/nRF24/RF24Ethernet/commit/8c6b3a25b65a5dfb5e146a0c398706e6b128bdbb)
- Clean up examples and config, add new example by \@TMRh20 in [`4490082`](https://github.com/nRF24/RF24Ethernet/commit/4490082d515467763c124f512f274ebf4c537c25)
- Merge remote-tracking branch 'origin/master_dev' by \@TMRh20 in [`1de5950`](https://github.com/nRF24/RF24Ethernet/commit/1de595010700fbba6981bb749dc26a7bcd14abd9)
- Minor adjustments by \@TMRh20 in [`c1732e9`](https://github.com/nRF24/RF24Ethernet/commit/c1732e951c0a67669e8cee5edbdf854da919939e)
- Merge remote-tracking branch 'origin/master_dev' by \@TMRh20 in [`13aa97d`](https://github.com/nRF24/RF24Ethernet/commit/13aa97ded8acc4db76ecbe83cf48901d18d64783)
- Changes for SLIP & TUN connectivity by \@TMRh20 in [`43da312`](https://github.com/nRF24/RF24Ethernet/commit/43da312c4f496ebf45b50c05677f82dc370d6d12)
- Update SLIP_Gateway example to make it clearer by \@TMRh20 in [`c3ee51f`](https://github.com/nRF24/RF24Ethernet/commit/c3ee51f0f3eb6d531da7bf8469721097ccd22176)
- Minor changes by \@TMRh20 in [`8e19e5f`](https://github.com/nRF24/RF24Ethernet/commit/8e19e5fec5bceaf62e40fc107270139c0d8e8fb7)
- MAC address format change by \@TMRh20 in [`5cfb3be`](https://github.com/nRF24/RF24Ethernet/commit/5cfb3be48ef7c5ffb7bdf75ce2b2ee569725d9ec)
- Increase TCP window re-open delay to .5 seconds by \@TMRh20 in [`000c283`](https://github.com/nRF24/RF24Ethernet/commit/000c283d8142eae9cf40cd34654b62892937ab7c)
- Check in current code improvements by \@TMRh20 in [`0de6fa4`](https://github.com/nRF24/RF24Ethernet/commit/0de6fa434928362ebe03a3d492e1f095bef54aab)
- Stability and reliability improvements by \@TMRh20 in [`bab410c`](https://github.com/nRF24/RF24Ethernet/commit/bab410c4f47ad20d8b492b8c41757e99e0dce868)
- Copied wrong HTML.h file by \@TMRh20 in [`66a8af2`](https://github.com/nRF24/RF24Ethernet/commit/66a8af2e8bb1902b01a48fafb378487997b0a744)
- Slight fix for HTML.h example file by \@TMRh20 in [`5b9d62e`](https://github.com/nRF24/RF24Ethernet/commit/5b9d62ef8611a80388b32bcee2af1901aea024b1)
- Update SimpleServer_Mesh example by \@TMRh20 in [`a7e83bd`](https://github.com/nRF24/RF24Ethernet/commit/a7e83bd3d144b3f729216e49bd2980c86604f66c)
- Separate buffers, periodic timing, cleanup by \@TMRh20 in [`85bfb66`](https://github.com/nRF24/RF24Ethernet/commit/85bfb66dfbaa61f3926264b18de609ec78410eb8)
- Update examples for waitAvailable() functionality by \@TMRh20 in [`4620209`](https://github.com/nRF24/RF24Ethernet/commit/4620209be5797a1acda9841d371c41362b20759a)
- Increment version, add info by \@TMRh20 in [`7fe284a`](https://github.com/nRF24/RF24Ethernet/commit/7fe284af38afef55ae1994d42060377baa33d42d)
- Improve TCP Window Handling by \@TMRh20 in [`b8da4d1`](https://github.com/nRF24/RF24Ethernet/commit/b8da4d1a17396e882b5852d4144f47eeca6a2e08)
- Cleanup from changes, Increment Version by \@TMRh20 in [`dfaafbd`](https://github.com/nRF24/RF24Ethernet/commit/dfaafbd22e7f8357eddb00bb26be5dec9e25f4be)
- Slight bugfix for v1.3b by \@TMRh20 in [`07d6c39`](https://github.com/nRF24/RF24Ethernet/commit/07d6c39da505804d37744ebd75938f8a363fd6dd)
- Slight increase to UIP_MAXRTX value by \@TMRh20 in [`72a6821`](https://github.com/nRF24/RF24Ethernet/commit/72a68218eaf6e5f09cf4b92ef26db871c190d358)
- Better handling of outgoing data by \@TMRh20 in [`18fb804`](https://github.com/nRF24/RF24Ethernet/commit/18fb804902b39edec688bf458450731b9ee4aa3d)
- Set multicastRelay in configure by \@TMRh20 in [`763c7df`](https://github.com/nRF24/RF24Ethernet/commit/763c7dfe30ec2e1fe3195195fe59ea7f1de9e843)
- Use ext RF24Network buffer for uIP (memory usage) by \@TMRh20 in [`da3ca3f`](https://github.com/nRF24/RF24Ethernet/commit/da3ca3f55f8937955ba85e06bf848d982014b64b)
- Rename example file by \@TMRh20 in [`d261122`](https://github.com/nRF24/RF24Ethernet/commit/d26112292af26ff788a3ef08fda225400a994284)
- Updates by \@TMRh20 in [`ad62631`](https://github.com/nRF24/RF24Ethernet/commit/ad62631321d3f4d534901ab767a22979f6ea4051)
- Major client fixes/workarounds, MQTT working by \@TMRh20 in [`87937bc`](https://github.com/nRF24/RF24Ethernet/commit/87937bc8d293d75b4ea04f9875f6a86646c6114f)
- Minor update to new mqtt example by \@TMRh20 in [`31f2184`](https://github.com/nRF24/RF24Ethernet/commit/31f21841598b12abf5348cc36b85fec8c1fa7b22)

[1.6]: https://github.com/nRF24/RF24Ethernet/compare/eb9a4bd7629afbcfa46f5e481330dfda1f5cd1d8...1.6

Full commit diff: [`eb9a4bd...1.6`][1.6]

<!-- generated by git-cliff -->
