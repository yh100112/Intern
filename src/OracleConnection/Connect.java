package OracleConnection;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class Connect {
	public static void main(String[] args) {
		String driver = "oracle.jdbc.driver.OracleDriver";
		String url = "jdbc:oracle:thin:@localhost:1521:oracle";
		String userid = "kdy";
		String passwd = "kdy";
		
		Connection con = null;
		
		try {
			Class.forName(driver);
			System.out.println("driver �ε� ����");
			
			con = DriverManager.getConnection(url,userid,passwd);
			System.out.println("����Ŭ ���� ����");
		}catch(ClassNotFoundException e) {
			System.out.println("driver �ε� ����");
		}catch(SQLException e) {
			System.out.println("����Ŭ ���� ����");
		}
		
		System.out.println("��!");
	}
}
