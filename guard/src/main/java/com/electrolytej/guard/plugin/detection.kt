package com.electrolytej.guard.plugin

/**
 * Copyright ® $ 2021
 * All right reserved.
 *
 * @author: jamesfchen
 * @email: hawksjamesf@gmail.com
 * @since: 五月/12/2021  星期三
 */
enum class Env(i: Int) {
    EMU(10), ROOT(11),
    MAL(12),//Malware
    ROM(13),//Dark System
    VIR_LOC(14),//Virtual Location
    PROXY(20), REMOTE(21),
    SIG(22),//package signature
    SANDBOX(23), XPOSED(24), DEBUG(25), CAMERA(26),
    UI_COUNT(27), UI_CHECK(28), UI_GET_LISTENER(29), UI_PROXY(30),
    ENV_DYN(31);
}