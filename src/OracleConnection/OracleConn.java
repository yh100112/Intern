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
			rs = pstmt.executeQuery(); // sql명령어 결과를 ResultSet 객체에 저장   select -> executeQuery() , update,delete,insert -> executeUpdate()
			
			while(rs.next()) { // 테이블 값들을 한 row씩 접근
				String name = rs.getString("name"); // column이 name인 곳의 데이터를 타입에 맞게 가져옴
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
			Class.forName(driver); // ojdbc8.jar에 있는 드라이버를 메모리에 로딩
			con = DriverManager.getConnection(url,userid,passwd); // 오라클 연동 (디비가 있는 url, 접속할 유저 아디 ,접속할 유저 비번)
			
			
			//select
			String sql = "select * from test";
			select_table(con,pstmt,rs);
			System.out.println("-----------------------------");
			
			//insert
			sql = "insert into test values(?,?)";
			pstmt = con.prepareStatement(sql);		// sql명령어를 실행해줄  PreparedStatement 객체
			pstmt.setString(1, "lee sun sin");
			pstmt.setInt(2, 26);
			int n = pstmt.executeUpdate();
			
			System.out.println("데이터 삽입 후 테이블 조회");
			select_table(con,pstmt,rs);
			
			System.out.println("-----------------------------");
			
			//update
			sql = "update test set age=44 where name='hong gil dong'";
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
























































