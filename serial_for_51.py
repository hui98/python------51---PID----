# coding:utf-8
import wx
from matplotlib.backends import backend_wxagg
from matplotlib.figure import Figure
import  struct
import  serial
import serial.tools.list_ports



#创建一个窗体类
class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, win,title='电机转速',size=(400,400))
        self.x = [0]
        self.y = [0]
        self.sympol =0
        self.panel = backend_wxagg.FigureCanvasWxAgg(self, -1, Figure())
        self.axes = self.panel.figure.gca()
        self.axes.cla()

    #清楚已经画了的点
    def cla(self):
        self.axes.cla()

    #绘制函数
    def plot(self,x,y):
        self.axes.plot(x, y)
        self.panel.draw()

    def dele(self):
        self.x = [0]
        self.y = [0]

def post(event):
#    sb=random.randint(0,10)
  # but.SetLabel(str(sb))

   # print float(text_P_input.GetValue())
    para_p = struct.pack('f',float(text_P_input.GetValue()))
    para_i = struct.pack('f',float(text_I_input.GetValue()))
    para_d = struct.pack('f',float(text_D_input.GetValue()))
    set_rate=struct.pack('H',int(text_Ecp_input.GetValue()))
    com_using=text_COM_input.GetValue()

    POST_COMMAND = '\x01'+para_p+para_i+para_d+set_rate
    #try:
    ser = serial.Serial(port=str(com_using), baudrate=9600)
    for i in range(0,15):
            ser.write(POST_COMMAND[i])

    ser.close()
    print 'serial open'
    ##   pass
 #   text_True_display.SetValue('1500')

   # test_P_input.SetLabel('0')

def OnTimer(event):
  #  text_True_display.SetValue(str(random.randint(0, 10)))


    com_using = text_COM_input.GetValue()

    if f.sympol == 1:
        #try:
        ser = serial.Serial(port=str(com_using), baudrate=9600, timeout=0.5)
        ser.write('\x02')
        count = ser.read(2)
        aaa = struct.unpack('H', count)[0]
        ser.close()
        #except:
          #  aaa=0
        text_True_display.SetValue(str(aaa/25))
        f.x.append(f.x[len(f.x)-1]+0.1)
        f.y.append(aaa/25)
        f.cla()
        f.plot(f.x,f.y)
  #  f.Refresh(True,None)
        print f.y
    else :
        text_True_display.SetValue('0')


def Palse(event):
    com_using = text_COM_input.GetValue()
    ser = serial.Serial(port=str(com_using), baudrate=9600, timeout=0.5)
    ser.write('\x03')
    if f.sympol == 1:
        f.sympol =0
        but.SetLabel('获取转速')
    else :
        f.sympol =1
        but.SetLabel('停止获取')
    ser.close()

def clear(event):
    f.cla()
    f.plot([], [])
    f.dele()

if __name__ == '__main__':
    app = wx.App()

#创建主副窗体
    win = wx.Frame(None,title='就叫辉的上位机吧',size=(410,340))
    win.Show()

    f = TestFrame()
    f.Show()

#以下是控件声明
    text_P = wx.StaticText(win,label='P',pos=(25,25),size=(80,80))
    text_I = wx.StaticText(win,label='I',pos=(25,105),size=(80,80))
    text_D = wx.StaticText(win,label='D',pos=(25,185),size=(80,80))
    text_Ecp = wx.StaticText(win,label='预设转速\n(r/min)',pos=(5,265),size=(80,80))
    text_True = wx.StaticText(win,label='实时转速\n(r/min)',pos=(210,125),size = (80,80))
    text_COM = wx.StaticText(win,label = '串口编号：',pos = (210,40),size = (80,80))

    text_P_input = wx.TextCtrl(win,pos=(55,25),size = (80,20))
    text_I_input = wx.TextCtrl(win,pos=(55,105),size = (80,20))
    text_D_input = wx.TextCtrl(win,pos=(55,185),size = (80,20))
    text_Ecp_input = wx.TextCtrl(win,pos=(65,265),size = (70,20))
    text_True_display = wx.TextCtrl(win,pos = (290,125),size = (80,40))
    text_COM_input = wx.TextCtrl(win,pos = (290,40),size = (80,25))


    but = wx.Button(win,label = '获取转速',pos = (200,220),size=(80,25))
    but.Bind(wx.EVT_BUTTON,Palse)

    qingchu =wx.Button(win,label = '清除数据',pos = (200,265),size=(80,25))
    qingchu.Bind(wx.EVT_BUTTON,clear)

    fasong =  wx.Button(win,label = 'POST',pos = (315,220),size=(60,60))
    fasong.Bind(wx.EVT_BUTTON,post)


#这里是定时触发的代码
    timer = wx.Timer(win)
    win.Bind(wx.EVT_TIMER, OnTimer, timer)
    timer.Start(10)

    port_list = list(serial.tools.list_ports.comports())
    if len(port_list) <= 0:
        pass
    else:
        port_list_0 = list(port_list[0])
        port_serial = port_list_0[0]
        text_COM_input.SetValue(str(port_serial))

    app.MainLoop()