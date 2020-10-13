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
#define N_TASK 5

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK TaskStartStk[TASK_STK_SIZE];
OS_STK TaskStk[N_TASK][TASK_STK_SIZE];
char TaskData[N_TASK];

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
OS_EVENT* Sem;//세마포어 생성 
OS_FLAG_GRP* s_grp;
OS_FLAG_GRP* r_grp;//이벤트 플래그 생성 
char receive_array[N_TASK - 1];// 각 랜덤 테스크 숫자 저장
int send_array[N_TASK - 1]; //디시션 테스크의 w/l을 저장한다

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void Task(void * data);
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
    OS_STK *ptos;
    OS_STK *pbos;
    INT32U  size;

	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);                        /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

	OSTaskCreate(TaskStart, (void*)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
   
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
	Sem = OSSemCreate(1);// 세마포어 초기화 함수 
		s_grp = OSFlagCreate(0x00, &err);//이벤트 플래그 초기화 함수 
		r_grp = OSFlagCreate(0x00, &err);
	for (i = 0; i < N_TASK; i++) {
		TaskData[i] = '0' + i;
		OSTaskCreate(Task, (void*)&TaskData[i], &TaskStk[i][TASK_STK_SIZE - 1], i + 1);
	}
}

void Task(void * pdata) {
	INT8U err;

	INT8U push_number;
	/*int get_number[4];*/
	
	INT8U i, j;

	INT8U min;		// 최솟값을 담을 변수
	INT8U min_task;	
	INT8U task_number = (int)(*(char*)pdata-48);//각 task의 index이다. pdata는 char타입이기 때문에 ascii 기준 -48을 하면 int형으로 바뀐다.

	char push_letter;//W OR L을 저장 Decision task에서 생성한다.
	int fgnd_color, bgnd_color;//배경색 random task 1~4가 w or l을 받았을때 화면에 칠해줄 색

	char s[10];//

	//task_number, pdata가 0-3이면 random task, 4이면 decision task
	if (*(char*)pdata == '4') {//decision task일 경우
		for (;;) {
			OSFlagPend(s_grp, 0x0F, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);
			min = send_array[0];
			min_task = 0;
			for (i = 1; i < N_TASK - 1; i++) {
				//random task가 보낸 4개의 랜덤 숫자중 가장 작은 수 찾기
				if (send_array[i] < min) {
					min = send_array[i];
					min_task = i;
				}
			}
			for (i = 0; i < N_TASK - 1; i++) {
				if (i == min_task) {//가장 작은 task에게는 w를 아닌 것에게는 l를 보낸다.
					push_letter = 'W';
				}
				else {
					push_letter = 'L';
				}
				receive_array[i] = push_letter;
				OSFlagPost(r_grp, (1 << i), OS_FLAG_SET, &err);
			}
			OSTimeDlyHMSM(0, 0, 5, 0);
		}
	}
	else {// 랜덤테스크가 1-4일경우 
		for (;;) {
			push_number = random(64);
			sprintf(s, "%2d", push_number);

			PC_DispStr(0 + 18 * task_number, 4, "task", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispChar(4 + 18 * task_number, 4, *(char*)pdata, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispStr(6 + 18 * task_number, 4, s, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

			OSSemPend(Sem, 0, &err);//크리티컬섹션 생성
			send_array[task_number] = push_number;//각 인덱스에 랜덤숫자자장
			OSFlagPost(s_grp, (1 << task_number), OS_FLAG_SET, &err);//쉬프트를 이ㅛㅇ해 
			//0~3->1~4로 변환한다 .
			//send배열에 저장했음을 이벤트 flag를통해 알린다 .
			OSSemPost(Sem);
			//rpg 의 값이 0x0f가 될 때까지 wait 한다
			OSFlagPend(r_grp, (1 << task_number), OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
			OSSemPend(Sem, 0, &err);//테스크 우선순위 색결정
			if (*(char*)pdata == '0') {
				bgnd_color = DISP_BGND_RED;
				fgnd_color = DISP_FGND_RED;
			}
			else if (*(char*)pdata == '1') {
				bgnd_color = DISP_BGND_CYAN;
				fgnd_color = DISP_FGND_CYAN;
			}
			else if (*(char*)pdata == '2') {
				bgnd_color = DISP_BGND_BLUE;
				fgnd_color = DISP_FGND_BLUE;
			}
			else if (*(char*)pdata == '3') {
				bgnd_color = DISP_BGND_GREEN;
				fgnd_color = DISP_FGND_GREEN;
			}
			PC_DispStr(8 + 18 * task_number, 4, "[", DISP_FGND_BLACK + bgnd_color);
			PC_DispStr(10 + 18 * task_number, 4, "]", DISP_FGND_BLACK + bgnd_color);
			PC_DispChar(9 + 18 * task_number, 4, receive_array[task_number], DISP_FGND_BLACK + bgnd_color);

			if (receive_array[task_number] == 'W') {
				for (j = 5; j < 24; j++) {
					for (i = 0; i < 80; i++) {
						PC_DispChar(i, j, ' ', fgnd_color + bgnd_color);
					}
				}
			}
			OSSemPost(Sem);
			OSTimeDlyHMSM(0, 0, 3, 0);
		}
	}
}

