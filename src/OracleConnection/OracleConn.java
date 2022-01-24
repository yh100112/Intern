package OracleConnection;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class OracleConn {
	
	public static void select_table(Connection con,PreparedStatement pstmt,ResultSet rs) {
		try {
			String sql = "select * from test";
			pstmt = con.prepareStatement(sql);
			rs = pstmt.executeQuery(); // sql��ɾ� ����� ResultSet ��ü�� ����   select -> executeQuery() , update,delete,insert -> executeUpdate()
			
			while(rs.next()) { // ���̺� ������ �� row�� ����
				String name = rs.getString("name"); // column�� name�� ���� �����͸� Ÿ�Կ� �°� ������
				int age = rs.getInt("age");
				
				System.out.println(name + "\t" + age);
			}	
			
		}catch(SQLException e) {
			e.printStackTrace();
		}
	}
	
	
	public static void main(String[] args) {
		String driver = "oracle.jdbc.driver.OracleDriver";
		String url = "jdbc:oracle:thin:@localhost:1521:oracle";
		String userid = "kdy";
		String passwd = "kdy";
		
		Connection con = null;
		PreparedStatement pstmt = null;
		ResultSet rs = null;
		
		try {
			Class.forName(driver); // ojdbc8.jar�� �ִ� ����̹��� �޸𸮿� �ε�
			con = DriverManager.getConnection(url,userid,passwd); // ����Ŭ ���� (��� �ִ� url, ������ ���� �Ƶ� ,������ ���� ���)
			
			
			//select
			String sql = "select * from test";
			select_table(con,pstmt,rs);
			System.out.println("-----------------------------");
			
			//insert
			sql = "insert into test values(?,?)";
			pstmt = con.prepareStatement(sql);		// sql��ɾ ��������  PreparedStatement ��ü
			pstmt.setString(1, "lee sun sin");
			pstmt.setInt(2, 26);
			int n = pstmt.executeUpdate();
			
			System.out.println("������ ���� �� ���̺� ��ȸ");
			select_table(con,pstmt,rs);
			
			System.out.println("-----------------------------");
			
			//update
			sql = "update test set age=44 where name='hong gil dong'";
			pstmt = con.prepareStatement(sql);
			int num = pstmt.executeUpdate();
			
			System.out.println("������Ʈ �� ���̺� ��ȸ");
			select_table(con, pstmt, rs);
			
			
		}catch(ClassNotFoundException e) {
			System.out.println("jdbc driver �ε� ����");
		}catch(SQLException e) {
			System.out.println("����Ŭ ���� ����");
		}finally {
			try {
				if(rs != null) rs.close();
				if(pstmt != null) pstmt.close();
				if(con != null) con.close();
			}catch(SQLException e) {
				e.getStackTrace();
			}
		}
	}
}
























































