# Blue-Bridge-Cup-embedded

话少，是不可能的，下面是对上述赛题的的一些讲解（不能算讲解，称之为理解吧）

首先来说说我程序的实现步骤吧：

1. 实现密码输入与判断密码的正误
2. 使用串口实现密码的更改
3. 实现PWM的输出
4. 实现题目中其他外设
5. 调试程序bug，并完善程序

### 时钟配置

<img src="C:\Users\KaiXin\AppData\Roaming\Typora\typora-user-images\image-20230316181222284.png" alt="image-20230316181222284" style="zoom:100%;" />

 上面是关于赛题的时钟配置，我们在学习F1平台的时候会配置HCLK为72MHz（这里我理解是将时钟配置成最大，速度快一点），这里配置80MHz我也不知道为啥，对这方面有兴趣的朋友可以分享分享。在debug配置的时候，最好将SW选上，在使用F1平台时候，刚开始使用hal库，由于debug未勾选上，导致芯片下不了程序，但在G4平台中网上的一些视频也没有勾选debug配置，并不会导致芯片锁死。官方给的驱动代码也没有选择。所以对G4来说并没有什么影响，各位兄弟在配置时候还是住点意吧。也有可能芯片是盗版才导致锁死的。

### 三行按键的理解

```C
    uc_KeyDown = uc_KeyVal & (uc_KeyOld ^ uc_KeyVal);  // Press the key value
    uc_KeyUp = ~ uc_KeyVal & (uc_KeyOld ^ uc_KeyVal);  // Press the key to lift the key value
    uc_KeyOld = uc_KeyVal;
```

现在我们开始假设 

**过程1**：**假设第4个被按下**也就是'uc_KeyVal  = 4(0000 0100)' 经过'uc_KeyDown' 中的运算，使得'uc_KeyDown' 变成了4（0000 0100） uc_KeyUp为0，uc_KeyOld = 4（0000 0100）

**过程2**：当第4个按键产生下降沿时，按键一直被按下（注意此时的状态是按下，并非按键的不定态）。此时uc_KeyVal  仍然为4（0000 0100）由于在uc_KeyOld 在上一次值的变换，当前的uc_KeyDown变成了0，uc_KeyUp也变成了0，uc_KeyOld还是4

**过程3**：第4个按键一直按下随后弹起，uc_KeyVal  变为0 。uc_KeyDown变为0，uc_KeyUp变成了4，uc_KeyOld 变为了0。按键识别结束。



### 定时器的配置

+ 定时器6用于计时5s

<img src="C:\Users\KaiXin\AppData\Roaming\Typora\typora-user-images\image-20230316195817201.png" alt="image-20230316195817201" style="zoom:80%;" />

+ 定时器2用来产生PWM脉冲

<img src="C:\Users\KaiXin\AppData\Roaming\Typora\typora-user-images\image-20230316200114859.png" alt="image-20230316200114859" style="zoom:80%;" />

初始设置频率是1000Hz = 80MHz x10^6 /[(79 + 1) + (999 + 1)]

对于上述定时器，6开中断进行精准定时，2不需要开启中断。

下面是定时器2设置PWM输出所用的函数。

```c
__HAL_TIM_SET_AUTORELOAD(&htim2, 999);  /* 设置频率1Khz */ 
__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500); /* 设置占空比为50% = 500/1000 */
```

到这里我们就结束了，代码里有详细的注释，可以直接下载进行参考，欢迎各位Up进行评论和指正。评论还是在csdn吧，GitHub还是个小白，不咋玩的明白。