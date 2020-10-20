/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #2
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */
#define N_TASK 4

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

/*OS_STK TaskStartStk[TASK_STK_SIZE];
OS_STK TaskStk[N_TASK][TASK_STK_SIZE];
/*
char TaskData[N_TASK]*/
//OS_EVENT randomsem;
//
//
//OS_EVENT* mbox_to_random[4];
//OS_EVENT* mbox_to_decision[4];
//
//OS_EVENT* queue_to_random;
//OS_EVENT* queue_to_decision;
//
//void * decision_queue[4];
//void * random_queue[4];//random task에서 decision task로 보내는queue의 배열
//  
//INT8U select=1; // 메일박스일때는 1 메시지 큐일때는 
char TaskData[N_TASK - 1];//각 task의 우선순위 입력
OS_STK Task_D_Stk[TASK_STK_SIZE]; //decision task의 생성시 쓰이는 스택 
OS_STK Task_Vote[N_TASK - 1][TASK_STK_SIZE]; //voting task의 생성시 쓰이는 스택
OS_STK Task_Start_Stk[TASK_STK_SIZE];

OS_EVENT* Rand_Sem; // 세마포어 변수 선언
OS_FLAG_GRP* v_grp;
OS_FLAG_GRP* d_grp; // 이벤트 플래그
OS_EVENT* q_master;
OS_EVENT* Mbox_Vote[N_TASK - 1];

INT8U majority;	//0이 나온 횟수
void* Master_q[N_TASK - 1]; //queue 사용 배열
//char receive_array[N_TASK - 1];// 각 랜덤 테스크 숫자 저장
//int send_array[N_TASK - 1]; //디시션 테스크의 w/l을 저장한다

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void V_Task(void* pdata);
void D_Task(void* pdata);
static  void  TaskStart(void *data);                  /* Function prototypes of tasks                  */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);
   

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main (void)
{
	INT8U err;
	INT8U i;
	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);                        /* Clear the screen                         */

    OSInit(); /* Initialize uC/OS-II                      */

	Rand_Sem = OSSemCreate(1);
	v_grp = OSFlagCreate(0x00, &err);
	d_grp = OSFlagCreate(0x00, &err);
	q_master = OSQCreate(Master_q, 3);
	for ( i = 0; i < 3; i++) {
		Mbox_Vote[i] = OSMboxCreate(0);
	}

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

	OSTaskCreate(TaskStart, (void*)0, &Task_Start_Stk[TASK_STK_SIZE - 1], 0);
   
    OSStart();                                             /* Start multitasking                       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

static void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    INT16S     key;
	INT8U i;

    pdata = pdata;                                         /* Prevent compiler warning                 */

    TaskStartDispInit();                                   /* Setup the display                        */

    OS_ENTER_CRITICAL();                                   /* Install uC/OS-II's clock tick ISR        */
    PC_VectSet(0x08, OSTickISR);
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

	//if (select == 1) {
	//	// 총 4개의 mailbox OSMboxCreate 사용해서 생성 (your code)
	//	for (i = 0; i < 4; i++)
	//	{
	//		mbox_to_decision[i] = OSMboxCreate(0);
	//		mbox_to_random[i] = OSMboxCreate(0);
	//	}
	//}
	//else if (select == 2) {
	//	// 크기가 4인 queue OSQCreate사용해서 생성 (your code)
	//	queue_to_decision = OSQCreate(decision_queue, 4);
	//	queue_to_random = OSQCreate(random_queue, 4);
	//	
	//}
  
    TaskStartCreateTasks();                                /* Create all other tasks                   */

    for (;;) {
        TaskStartDisp();                                   /* Update the display                       */

        if (PC_GetKey(&key)) {                             /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Yes, return to DOS                       */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDly(OS_TICKS_PER_SEC);                       /* Wait one second                          */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDispInit (void)
{
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr( 0,  1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  3, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 20, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 21, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 22, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 23, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDisp (void)
{
    char   s[80];


    sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */
    PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */
    PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */
    PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "V%4.2f", (float)OSVersion() * 0.01);               /* Display uC/OS-II's version number    */
    PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    switch (_8087) {                                               /* Display whether FPU present          */
        case 0:
             PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 1:
             PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 2:
             PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 3:
             PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
	INT8U i;
	INT8U err;//에러 표시
	//Sem = OSSemCreate(1);// 세마포어 초기화 함수 
	//	s_grp = OSFlagCreate(0x00, &err);//이벤트 플래그 초기화 함수 
	//	r_grp = OSFlagCreate(0x00, &err);
	for ( i = 0; i < N_TASK-1; i++) {
		TaskData[i] =  i;
		OSTaskCreate(V_Task, (void*)&TaskData[i], &Task_Vote[i][TASK_STK_SIZE - 1], i + 1);
	}
	OSTaskCreate(D_Task, (void*)0, &Task_D_Stk[TASK_STK_SIZE - 1], i + 1);
}

void V_Task(void* pdata) {
	INT8U i;
	INT8U err;
	INT8U rand_num;
	INT8U task_number;
	INT8U result;
	char c;

	for (;;) {
		task_number = *(int*)pdata; // task번호 생성
		rand_num = rand();
		if (rand_num%2 == 0) c = 'O';
		else c = 'X';
		OSQPost(q_master, &c); //queue에 Master task에게 (voting task 중 하나)결과를 전송
		OSFlagPend(v_grp, 0x07, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err); // decision task에서 master를 결정할 때까지 대기
		result = *(int*)OSMboxPend(Mbox_Vote[task_number], 0, &err); // master가 누구인지 decision  task에서 받아옴
		if (result == task_number) { //master task가 할 일
			OSSemPend(Rand_Sem, 0, &err);
			majority = 0;
			for ( i = 0; i < N_TASK - 1; i++) {
				if ((*(char*)(OSQPend(q_master, 0, &err))) == 'O') {
					majority++; //O이 나온 횟수 카운트
				}
				PC_DispChar(30, 4, '0' + majority, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
			OSFlagPost(d_grp, 0x01 << result, OS_FLAG_SET, &err); //decision task로 작업이 끝났음을 알린다.
			OSSemPost(Rand_Sem);
		}
		OSTimeDlyHMSM(0, 0, 2, 0);
	}
}

void D_Task(void* pdata) {
	INT8U i, j;
	INT8U master;
	INT8U err;
	INT8U color;
	for (;;) {
		master = rand() % 3;
		OSMboxPost(Mbox_Vote[0], &master);
		OSMboxPost(Mbox_Vote[1], &master);
		OSMboxPost(Mbox_Vote[2], &master);
		OSFlagPost(v_grp, 0x01 << master, OS_FLAG_SET, &err);
		OSFlagPend(d_grp, 0x07, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err); //master task 작업 끝날 때까지 대기

		OSSemPend(Rand_Sem, 0, &err);
		if (majority > 1)  // O이 많으면 파랑(O가 2개,3개)
		color = DISP_BGND_BLUE;
		else  color = DISP_BGND_RED;
		for ( i = 0; i < N_TASK - 1; i++) 
		PC_DispChar(9 * i, 4, *(char*)Master_q[i], DISP_FGND_WHITE + DISP_BGND_BLACK);
		for (j = 5; j < 20; j++) for (i = 0; i < 80; i++) PC_DispChar(i, j, ' ', DISP_FGND_WHITE + color);
		OSSemPost(Rand_Sem);

		OSTimeDlyHMSM(0, 0, 3, 0);
	}
}

