<%@ page language="java" contentType="text/html; charset=EUC-KR"
    pageEncoding="EUC-KR" import="java.sql.*"%>
<%
	Class.forName("oracle.jdbc.driver.OracleDriver");

	Connection conn = DriverManager.getConnection("jdbc:oracle:thin:@localhost:1521:oracle","kdy","kdy");
	
	PreparedStatement stmt = conn.prepareStatement("insert into custom_01 values(?,?,?,?,?)");
	
	stmt.setString(1, request.getParameter("p_id"));
	stmt.setString(2, request.getParameter("p_pw"));
	stmt.setString(3, request.getParameter("c_name"));
	stmt.setString(4, request.getParameter("c_email"));
	stmt.setString(5, request.getParameter("c_tel"));
	
	stmt.executeUpdate();
%>

<!DOCTYPE html>
<html>
<head>
<meta charset="EUC-KR">
<title>Insert title here</title>
</head>
<body>
	<a href="add.jsp">등록화면</a>
</body>
</html>






