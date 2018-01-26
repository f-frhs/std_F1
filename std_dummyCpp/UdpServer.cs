using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace std_dummyCpp
{
    /// <summary> 応答のみダミーサーバ(c++の代用) </summary>
    class DummyUdpServer
    {
        static void Main(string[] args)
        {
            //サーバ名
            var PipeName = "TestPipe1";

            //名前付きパイプ：サーバ
            //NamedPipeServerStream stream;

            //クライアントからの接続をまつ
            var stream = new NamedPipeServerStream(PipeName, PipeDirection.InOut);           
            stream.WaitForConnection();

            var isConnect = stream.IsConnected;

            //タイムアウト設定
            if (stream.CanTimeout)
            {
                stream.WriteTimeout = 1000;
                stream.ReadTimeout = 1000;
            }

            //Thread.Sleep(5000);

            var reader = new StreamReader(stream);

           //var task = Task.Run(()=>Reader(reader));
           var result = reader.ReadLine();
            Console.WriteLine(result);
           //var result  = task.Result;

            var aa = 10;

            var recvBuf = new byte[stream.InBufferSize];
            var cnt = stream.Read(recvBuf, 0, recvBuf.Length);
            var rStr = Encoding.UTF8.GetString(recvBuf, 0, cnt);

            var a = 10;

            //送信：接続完了
            var sendConectStr = "TestPipe1,connect";
            var cmdBytes = Encoding.UTF8.GetBytes(sendConectStr);
            stream.Write(cmdBytes, 0, cmdBytes.Length);

            Console.WriteLine("aaa");
            Console.ReadLine();

        }

        private static async Task<string> Reader(StreamReader reader)
        {
            var temp = await reader.ReadLineAsync();
            return temp;
        }

        /// <summary> 名前付きパイプ接続待ち </summary>
        /// <param name="stream"></param>
        //private static async void ConnectFromClient(NamedPipeServerStream stream)
        //{
            //var connection = await Task.Run(() =>
            //{
                //stream.WaitForConnection();
                //var sendConectStr = "TestPipe1,connect";
                //SendData(stream, Encoding.UTF8.GetBytes(sendConectStr));
                //return true;
            //});

            //do
            //{
            //    if (connection)
            //    {
            //        Console.WriteLine($"パイプ接続完了", "Debug");
            //    }
            //    else
            //    {
            //        Console.WriteLine($"パイプ接続失敗,カメラプログラムの起動・設定を確認してください", "Error");
            //    }
            //} while (!connection);

            //return;
        //}

        /// <summary>データ送信 </summary>
        /// <param name="pipe">送信するパイプ</param>
        /// <param name="cmd">送信するコマンド</param>
        /// <returns></returns>
        //private static void SendData(NamedPipeServerStream pipe, byte[] cmdBytes)
        //{
        //    pipe.Write(cmdBytes, 0, cmdBytes.Length);
        //}
    }
}
