/*
 * Jumping to 0x0000 will restart the whole program
 */
void reboot_cpu()
{
  void (*reboot)() = 0x0000;
  (*reboot)();
}