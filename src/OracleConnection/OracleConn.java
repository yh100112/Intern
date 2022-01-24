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
			rs = pstmt.executeQuery();
			
			while(rs.next()) {
				String name = rs.getString("name");
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
			Class.forName(driver);
			con = DriverManager.getConnection(url,userid,passwd);
			
			
			//select
			String sql = "select * from test";
			select_table(con,pstmt,rs);
			System.out.println("-----------------------------");
			
			//insert
			sql = "insert into test values(?,?)";
			pstmt = con.prepareStatement(sql);
			pstmt.setString(1, "hong gil dong");
			pstmt.setInt(2, 33);
			int n = pstmt.executeUpdate();
			
			System.out.println("데이터 삽입 후 테이블 조회");
			select_table(con,pstmt,rs);
			
			System.out.println("-----------------------------");
			
			//update
			sql = "update test set age=999 where name='hong gil dong'";
			pstmt = con.prepareStatement(sql);
			int num = pstmt.executeUpdate();
			
			System.out.println("업데이트 후 테이블 조회");
			select_table(con, pstmt, rs);
			
			
		}catch(ClassNotFoundException e) {
			System.out.println("jdbc driver 로딩 실패");
		}catch(SQLException e) {
			System.out.println("오라클 연동 실패");
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
























































