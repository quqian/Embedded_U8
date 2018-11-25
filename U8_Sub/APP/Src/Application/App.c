#include "APP.h"



void APP_Init(void)
{

}




using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Globalization;
namespace HextoBin
{
    public partial class Form1 : Form
    {
        OpenFileDialog file = new OpenFileDialog();
        SaveFileDialog Savefile = new SaveFileDialog();
        FolderBrowserDialog dialog = new FolderBrowserDialog();
        private string foldPath;
        private string szHex;
       // private string Name;
        //private string Path;
 
        public Form1()
        {
            InitializeComponent();
        }
 
        private void button1_Click(object sender, EventArgs e)
        {
            file.Filter = "二进制文件(*.hex)|*.hex";//文件过滤器
            //记忆上次打开的目录
            file.RestoreDirectory = true;
            //设置是否允许多选
            file.Multiselect = false;
            if (file.ShowDialog() == DialogResult.OK)
            {
                //显示文件名和拓展名
               // textBox1.Text = System.IO.Path.GetFileName(file.SafeFileName);
                textBox1.Text = file.FileName;
            }
        }
        private void button2_Click(object sender, EventArgs e)
        {
            if (textBox1.Text != "")
            {
                StreamReader HexReader = new StreamReader(file.FileName);         
                try
                {
                    while (true)
                    {
                        string szLine = HexReader.ReadLine(); //读取一行数据
                        if (szLine == null) //读完所有行
                        {
                            break;
                        }
                        if (szLine.Substring(0, 1) == ":") //判断第1字符是否是:
                        {
 
                            if (szLine.Substring(1, 8) == "00000001")//数据结束
                            {
                                
                                break;
                            }
                            if ((szLine.Substring(1, 1) == "1") ||(szLine.Substring(3,4)!="0000"&&szLine.Substring(1, 1) == "0"))
/**/
                            {
                                szHex += szLine.Substring(9, szLine.Length - 11); //读取有效字符：后0和1
                            }
                        }
                    }
                }
                catch
                { }
                Int32 i;
                Int32 j = 0;
                Int32 Length = szHex.Length;
                byte[] szBin = new byte[Length / 2];
                for (i = 0; i < Length; i += 2) //两字符合并成一个16进制字节
                {
                    szBin[j] = (byte)Int16.Parse(szHex.Substring(i, 2), NumberStyles.HexNumber);
                    j++;
 
                }
                if (textBox2.Text != "")
                {
                    try
                    {
                        FileStream fs = new FileStream(Savefile.FileName, FileMode.Create);
                        //FileStream fs =File.Create(foldPath);                      
                        BinaryWriter bw = new BinaryWriter(fs);
                        for (int k = 0; k < j; k++)
                        {
                            bw.Write(szBin[k]);                          
                        }                      
                        bw.Close();
                        fs.Close();
                        MessageBox.Show("转换完成","提示");
                    }
                    catch
                    { }
 
                }
                else
                {
                    MessageBox.Show("请选择bin文件保存路径","提示");
                }
 
 
            }
            else
            {
                MessageBox.Show("请选择一个Hex文件", "提示");
            }
          
            
 
        }
 
        private void button3_Click(object sender, EventArgs e)
        {
 
           
            Savefile.RestoreDirectory = true;
            Savefile.Filter = "Bin文件(*.bin)|*.bin";
            if (Savefile.ShowDialog() == DialogResult.OK && Savefile.FileName.Length > 0)
            {
                textBox2.Text = Savefile.FileName;
                //textBox2.Text= Savefile.FileName.Substring(Savefile.FileName.LastIndexOf("\\") + 1);//文件名+后缀
               // textBox2.Text = Savefile.FileName.Substring(0,Savefile.FileName.LastIndexOf("\\"));//路径
            }
 
 
            
          /*  if (dialog.ShowDialog() == DialogResult.OK)
            {
                 foldPath = dialog.SelectedPath;             
                DirectoryInfo theFolder = new DirectoryInfo(foldPath);
                FileInfo[] dirInfo = theFolder.GetFiles();
                //遍历文件夹
                foreach (FileInfo file in dirInfo)
                {
                    MessageBox.Show(file.ToString());
                }
                textBox2.Text = foldPath;
                       
            }*/
            
 
        }
 
        
    }
}










