#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <Windows.h>
namespace CLIKER
{
   using namespace std;
   auto handle = GetStdHandle(STD_INPUT_HANDLE);
   INPUT_RECORD inputRecord{};
   void UpdateInput()
   {
      DWORD numRead;
      ZeroMemory(&inputRecord, sizeof(inputRecord));
      if (!PeekConsoleInput(handle, &inputRecord, 1, &numRead))
      {
         cout << "error\n";
         cout << "last error: " << GetLastError() << '\n';
         system("pause");
         exit(-1);
      }
      FlushConsoleInputBuffer(handle);
   }
   bool EscapePressed()
   {
      return inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE && inputRecord.Event.KeyEvent.bKeyDown == 1;
   }
   struct Command
   {
      enum Type
      {
         MouseMove, MouseDown, MouseUp, Wait
      };
      Type type;
      Command(Type type) : type(type)
      {
      }
      virtual bool Execute() = 0;
   };
   struct MouseMove : Command
   {
      int x, y;
      MouseMove(int x, int y) : x(x), y(y), Command(Type::MouseMove)
      {
      }
      bool Execute() override
      {
         cout << "MouseMove " << x << ' ' << y << '\n';
         SetCursorPos(x, y);
         UpdateInput();
         return !EscapePressed();
      }
   };
   struct MouseDown : Command
   {
      MouseDown() : Command(Type::MouseDown)
      {
      }
      bool Execute() override
      {
         cout << "MouseDown\n";
         mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
         UpdateInput();
         return !EscapePressed();
      }
   };
   struct MouseUp : Command
   {
      MouseUp() : Command(Type::MouseUp)
      {
      }
      bool Execute() override
      {
         cout << "MouseUp\n";
         mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
         UpdateInput();
         return !EscapePressed();
      }
   };
   struct Wait : Command
   {
      int ms;
      Wait(int ms) : ms(ms), Command(Type::Wait)
      {
      }
      bool Execute() override
      {
         cout << "Wait " << ms;
         while (ms > 1000)
         {
            Sleep(1000);
            ms -= 1000;
            cout << '.';
            UpdateInput();
            if (EscapePressed())
            {
               return false;
            }
         }
         Sleep(ms);
         cout << '\n';
         UpdateInput();
         return !EscapePressed();
      }
   };
   vector<Command*> commands;
   //void ExecuteCommands()
   //{
   //   commands.push(new MouseMove(1811, 973));
   //   commands.push(new MouseDown());
   //   commands.push(new Wait(100));
   //   commands.push(new MouseUp());
   //   commands.push(new Wait(500));
   //   commands.push(new MouseMove(1880, 965));
   //   commands.push(new MouseDown());
   //   commands.push(new Wait(100));
   //   commands.push(new MouseUp());
   //   commands.push(new Wait(500));
   //   commands.push(new MouseMove(1826, 856));
   //   commands.push(new MouseDown());
   //   commands.push(new Wait(100));
   //   commands.push(new MouseUp());
   //   commands.push(new Wait(130000));
   //   bool stopExecution = false;
   //   while (!commands.empty())
   //   {
   //      auto c = commands.front();
   //      if (!c->Execute())
   //      {
   //         stopExecution = true;
   //         break;
   //      }
   //      delete c;
   //      commands.pop();
   //   }
   //}
   HWND window;
   void setw()
   {
      string input;
      cin >> input;
      window = FindWindow(nullptr, input.c_str());
      if (window)
         cout << "Success!\n";
      else
         cout << "Fail!\n";
   }
   void setwpos()
   {
      string input;
      int x, y, w, h;
      try
      {
         cin >> input; x = stoi(input);
         cin >> input; y = stoi(input);
         cin >> input; w = stoi(input);
         cin >> input; h = stoi(input);
      }
      catch (exception e)
      {
         cout << "Failed to convert " << input << " to int\n";
         cout << e.what();
         return;
      }
      if (SetWindowPos(window, nullptr, x, y, w, h, 0))
         cout << "Success!\n";
      else
         cout << "Fail!\n";
   }
   void qadd()
   {
      string input;
      cin >> input;
      transform(input.begin(), input.end(), input.begin(), tolower);
      if (input == "help")
      {
         cout << "MouseMove 'x' 'y'\n";
         cout << "MouseDown\n";
         cout << "MouseUp\n";
         cout << "Wait 'ms'\n";
      }
      if (input == "mousemove")
      {
         int x, y;
         try
         {
            cin >> input; x = stoi(input);
            cin >> input; y = stoi(input);
         }
         catch (exception e)
         {
            cout << "Failed to convert " << input << " to int\n";
            cout << e.what();
            return;
         }
         commands.push_back(new MouseMove(x, y));
         cout << "Success!\n";
         return;
      }
      if (input == "mousedown")
      {
         commands.push_back(new MouseDown);
         cout << "Success!\n";
         return;
      }
      if (input == "mouseup")
      {
         commands.push_back(new MouseUp);
         cout << "Success!\n";
         return;
      }
      if (input == "wait")
      {
         int ms;
         try
         {
            cin >> input; ms = stoi(input);
         }
         catch (exception e)
         {
            cout << "Failed to convert " << input << " to int\n";
            cout << e.what();
            return;
         }
         commands.push_back(new Wait(ms));
         cout << "Success!\n";
         return;
      }
   }
   void qsee()
   {
      for (auto& e : commands)
      {
         switch (e->type)
         {
         case Command::MouseMove:
         {
            cout << "MouseMove ";
            auto p = (MouseMove*)e;
            cout << p->x << ' ' << p->y;
            break;
         }
         case Command::MouseUp:
         {
            cout << "MouseUp";
            break;
         }
         case Command::MouseDown:
         {
            cout << "MouseDown";
            break;
         }
         case Command::Wait:
         {
            cout << "Wait ";
            auto p = (Wait*)e;
            cout << p->ms;
            break;
         }
         default:

            break;
         }
         cout << '\n';
      }
   }
   void exec()
   {
      for (auto& c : commands)
      {
         if (!c->Execute())
         {
            break;
         }
      }
      cout << "Finished!\n";
   }
   bool ReadUserInput()
   {
      string input;
      cin >> input;
      while (input[0] == ' ')
         input.erase(input.begin());
      if (input == "quit")
         return false;

      int execCount = 1;
      if (input.front() == '(')
      {
         if (input.back() == ')')
         {
            input.erase(input.begin());
            input.erase(input.end() - 1);
            try
            {
               execCount = stoi(input);
            }
            catch (exception e)
            {
               cout << "Failed to convert " << input << " to int\n";
               cout << e.what();
               return true;
            }
            cin >> input;
            while (input[0] == ' ')
               input.erase(input.begin());
            if (input == "quit")
               return false;
         }
      }

#define regcommand(name) if(input == #name){ while(execCount--)name(); return true; }
      regcommand(setw);
      regcommand(setwpos);
      regcommand(qadd);
      regcommand(qsee);
      regcommand(exec);
#undef regcommand
      cout << "Unknown command " << input << "!\n";
      return true;
   }
   void Run()
   {

      cout << "\t+-------------------+\n";
      cout << "\t|Welocome to CLIKER!|\n";
      cout << "\t+-------------------+\n";
      cout << '\n';
      cout << "List of commands:\n";
      cout << "setw 'windowname' - set current window to be 'windowname'\n";
      cout << "setwpos 'x' 'y' 'w' 'h' - set current window to be in wanted position\n";
      cout << "qadd 'commandname' [arguments] - add a command to the queue\n";
      cout << "qsee - see commands in the queue\n";
      cout << "exec - execute commands from the queue\n";
      cout << "quit - quit\n";
      do
      {
         cout << ">>>";
      } while (ReadUserInput());
      //
      //system("pause");
      //
      //ExecuteCommands();
      //
      //system("pause");
   }
}
int main()
{
   CLIKER::Run();
}