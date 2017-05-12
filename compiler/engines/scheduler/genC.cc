#include <string.h>
#include <ctype.h>
#include <tmc.h>
#include "vnusbasetypes.h"

#include "genC.h"
#include "scheduler/Scheduling.H"
#include "dag/NoDupDAGNode.H"
#include "task.h"
#include "io.h"


tmstring c_file_name = "gen_sched.c";
tmstring h_file_name = "gen_sched.h";


void
print_C_schedule (void)
{
  FILE *c_sched_file, *h_sched_file;
  task_list_item_p t;
  int nid;


     /* Generate C file */
  c_sched_file = ckfopen (c_file_name, "w");

  fputs ("#include <stdio.h>\n\n", c_sched_file);
  fputs ("#include \"gen_RT.h\"\n", c_sched_file);
  fputs ("#include \"LU.h\"\n\n\n", c_sched_file);

  fputs ("PTask task_array[NR_TASKS] = {", c_sched_file);
  t = get_first_task();
  if (t != NULL)
  {
    fprintf (c_sched_file, "%s", get_task_name(t));
  
    for (t = get_next_task(t); t != NULL; t = get_next_task(t))
      fprintf (c_sched_file, ", %s", get_task_name(t));
  }
  fputs ("};\n", c_sched_file);

  fputs ("int task_nr_card[NR_TASKS] = {", c_sched_file);
  t = get_first_task();
  if (t != NULL)
  {
    fprintf (c_sched_file, "%d", get_task_nr_card(t));
  
    for (t = get_next_task(t); t != NULL; t = get_next_task(t))
      fprintf (c_sched_file, ", %d", get_task_nr_card(t));
  }
  fputs ("};\n\n\n", c_sched_file);


  fprintf (c_sched_file, "int task_prio[NR_NODES] = {%d",
           ((PNoDupDAGNode) dag->GetNode(0))->GetSeqNrOnProc());
  for (nid = 1; nid < dag->GetNrNodes(); nid++)
  {
    fprintf (c_sched_file, ", %d", 
             ((PNoDupDAGNode) dag->GetNode(nid))->GetSeqNrOnProc());
  }
  fputs ("};\n\n", c_sched_file);

  fprintf (c_sched_file, "int task_proc[NR_NODES] = {%d",
           ((PNoDupDAGNode) dag->GetNode(0))->GetProcID());
  for (nid = 1; nid < dag->GetNrNodes(); nid++)
  {
    fprintf (c_sched_file, ", %d", 
             ((PNoDupDAGNode) dag->GetNode(nid))->GetProcID());
  }
  fputs ("};\n\n", c_sched_file);

  fclose (c_sched_file);


     /* Generate H file */
  h_sched_file = ckfopen (h_file_name, "w");

  
  fputs ("#ifndef __GEN_RT_H\n#define __GEN_RT_H\n\n", h_sched_file);
  fputs ("#include \"LU.h\"\n\n\n", h_sched_file);
  fputs ("#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n", h_sched_file);

  fputs ("typedef void (*PTask)();\n\n", h_sched_file);

  fprintf (h_sched_file, "#define NR_TASKS %d", get_nr_tasks());

  fputs ("enum task_id {", h_sched_file);
  t = get_first_task();
  if (t != NULL)
  {
    char buf[100];
    strcpy (buf, get_task_name(t));
    for (uint k = 0; k < strlen(buf); k++)
      buf[k] = toupper(buf[k]);
    fprintf (h_sched_file, "%s", buf);
  
    for (t = get_next_task(t); t != NULL; t = get_next_task(t))
    {
      strcpy (buf, get_task_name(t));
      for (uint k = 0; k < strlen(buf); k++)
        buf[k] = toupper(buf[k]);
      fprintf (h_sched_file, ", %s", buf);
    }
  }
  fputs ("};\n\n", h_sched_file);

  fputs ("extern PTask task_array[NR_TASKS];\n", h_sched_file);
  fputs ("extern int task_nr_card[NR_TASKS];\n\n", h_sched_file);

  fprintf (h_sched_file, "#define MAX_NR_CARDS %d\n\n",
                         get_task_max_nr_cards());
  fprintf (h_sched_file, "#define NR_NODES %d\n\n", dag->GetNrNodes());
  
  fputs ("extern int task_prio[NR_NODES];\n", h_sched_file);
  fputs ("extern int task_proc[NR_NODES];\n\n", h_sched_file);

  fputs ("#ifdef __cplusplus\n}\n#endif\n\n", h_sched_file);
  fputs ("#endif\n", h_sched_file);

  fclose (h_sched_file);
}
