#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <debug.h>
#include <string.h>

static unsigned char padBuf[256] __attribute__((aligned(64)));

static void wait_for_pad(void)
{
    int state;
    padInit(0);
    padPortOpen(0, 0, padBuf);
    do {
        state = padGetState(0, 0);
    } while (state != PAD_STATE_STABLE && state != PAD_STATE_FINDCTP1);
}

static int cross_pressed(void)
{
    struct padButtonStatus buttons;
    if (padGetState(0, 0) != PAD_STATE_STABLE) return 0;
    if (padRead(0, 0, &buttons) == 0) return 0;
    return (buttons.btns & PAD_CROSS) == 0;
}

static void launch_opl(void)
{
    t_ExecData elf;
    int ret;

    scr_clear();
    scr_printf("PS2 GAME STATION\n\n");
    scr_printf("Starting OPL from USB...\n");
    scr_printf("mass:/OPNPS2LD.ELF\n");

    SifInitRpc(0);
    SifLoadFileInit();
    ret = SifLoadElf("mass:/OPNPS2LD.ELF", &elf);
    if (ret < 0) {
        scr_printf("\nERROR: OPL was not found.\n");
        scr_printf("Check USB and filename.\n");
        for (;;) {
            if (cross_pressed()) break;
        }
        return;
    }

    /* Release LOADFILE/SIF RPC services before handing control to OPL.
       This is required by the standard PS2 ELF-loader handoff sequence. */
    SifLoadFileExit();
    SifExitRpc();
    FlushCache(0);
    FlushCache(2);
    ExecPS2((void *)elf.epc, (void *)elf.gp, 0, NULL);
}

int main(int argc, char *argv[])
{
    init_scr();
    scr_clear();
    scr_printf("================================\n");
    scr_printf("       PS2 GAME STATION         \n");
    scr_printf("================================\n\n");
    scr_printf("USB OPL launcher\n\n");
    scr_printf("Connect USB containing:\n");
    scr_printf("OPNPS2LD.ELF\n\n");
    scr_printf("Press CROSS to open PS2 games.\n");

    wait_for_pad();
    for (;;) {
        if (cross_pressed()) {
            launch_opl();
            scr_clear();
            scr_printf("PS2 GAME STATION\n\n");
            scr_printf("Press CROSS to launch OPL.\n");
        }
    }
    return 0;
}
