
// vermouthのみ使用のCOMMNG-MIDI
// あまりに一緒すぎるんで 関数名変えてこっちへ


// ---- com manager midi for vermouth

#define	COMSIG_MIDI		0x4944494d

#ifdef __cplusplus
extern "C" {
#endif

void cmvermouth_initialize(void);
COMMNG cmvermouth_create(void);

void cmvermouth_load(UINT rate);
void cmvermouth_unload(void);

#ifdef __cplusplus
}
#endif

