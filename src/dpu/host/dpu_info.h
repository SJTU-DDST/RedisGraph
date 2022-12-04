#ifndef _DPU_INFO_H
#define _DPU_INFO_H

#ifndef NR_DPUS
#define NR_DPUS 128
#endif

#ifndef DPU_BINARY
#define DPU_BINARY "./dpu_task"
#endif

#define NR_SLAVE_TASKLETS (NR_TASKLETS - 1)

#endif