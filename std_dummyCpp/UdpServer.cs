using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Pipes;
using System.Linq;
using System.Text;
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

            //クライアントからの接続をまつ
            var stream = new NamedPipeServerStream(PipeName);           
            //ConnectFromClient(stream);
            stream.WaitForConnection();

            var recvBuf = new byte[stream.InBufferSize];
            var cnt = stream.Read(recvBuf, 0, recvBuf.Length);
            var rStr = Encoding.UTF8.GetString(recvBuf, 0, cnt);

            //送信：接続完了
            var sendConectStr = "TestPipe1,connect";
            SendData(stream, Encoding.UTF8.GetBytes(sendConectStr));

            Console.WriteLine("aaa");
            Console.ReadLine();

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
        private static void SendData(NamedPipeServerStream pipe, byte[] cmdBytes)
        {
            //送信関係デバッグ出力
            Trace.WriteLine($"送信文字列\n{cmdBytes}", "Debug");
            var sb = new StringBuilder();
            sb.AppendLine("送信バイト配列\n");
            foreach (var cmdByte in cmdBytes)
            {
                sb.Append($"{cmdByte:X2} ");
            }
            Trace.WriteLine($"{sb.ToString()}", "Debug");

            pipe.Write(cmdBytes, 0, cmdBytes.Length);
        }
    }
}
