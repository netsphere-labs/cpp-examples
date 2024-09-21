
using ClassLibrary1;

namespace ConsoleApp1
{

internal class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");

        using (var h = new FugaWrap("日本語のテキスト")) {
            Console.WriteLine("get = {0}", h.get());
        }
        Console.WriteLine("Here is after using()");
    }
}
}
