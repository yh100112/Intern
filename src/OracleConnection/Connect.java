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
			System.out.println("driver 로딩 성공");
			
			con = DriverManager.getConnection(url,userid,passwd);
			System.out.println("오라클 연동 성공");
		}catch(ClassNotFoundException e) {
			System.out.println("driver 로딩 실패");
		}catch(SQLException e) {
			System.out.println("오라클 연동 실패");
		}
		
		System.out.println("끝!");
	}
}
