#include "tasks.h"

#include "app.h"
#include "mem.h"
#include "os.h"
#include "state.h"
#include "ui.h"

void task_ui(void *args __attribute__((unused)))
{
  UI *ui = memalloc(sizeof(UI));
  ui_init(ui);

  while (true)
  {
    ui_clear(ui);
    app_handler(ev_ui, ui);
    ui_update(ui);
    os_sleep(50);
  }
  os_exit();
}
