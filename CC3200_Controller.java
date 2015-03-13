import java.net.*;
import java.io.*;
import java.util.Scanner;


public class CC3200_Controller{

    private static Socket socket;
    public static void main(String[] args) {
        try{
            int port = 1200;
            ServerSocket serverSocket = new ServerSocket(port);
            System.out.println("Server Started and listening to the port 1200");
            while(true){
                socket = serverSocket.accept();
                System.out.println("Clinet connected to the server");
                
                InputStream is = socket.getInputStream();
                InputStreamReader isr = new InputStreamReader(is);
                BufferedReader br = new BufferedReader(isr);                
                
                OutputStream os = socket.getOutputStream();
                OutputStreamWriter osw = new OutputStreamWriter(os);
                BufferedWriter bw = new BufferedWriter(osw);
                
                String rmessage;
                String smessage;
                
                Scanner s = new Scanner(System.in);                         
                while(true){
                    smessage = s.nextLine();                
                    bw.write(smessage);
                    bw.flush();
                    rmessage = br.readLine();
                    System.out.println(rmessage);
                }
            }
        }catch (Exception e){
            e.printStackTrace();
        }finally
        {
            try{
                socket.close();
            }
            catch(Exception e){
            }
        }
    }
}