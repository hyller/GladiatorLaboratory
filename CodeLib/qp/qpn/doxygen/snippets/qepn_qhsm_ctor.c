void Pelican_ctor(Pelican *me, uint8_t timeout) {
    QActive_ctor(&me->super, Q_STATE_CAST(&Pelican_initial));
    me->pedFlashCtr = timeout;
}
