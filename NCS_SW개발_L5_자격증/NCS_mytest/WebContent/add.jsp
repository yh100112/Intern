<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8" import="java.sql.*"%>
<%
	Class.forName("oracle.jdbc.driver.OracleDriver");
	Connection conn = DriverManager.getConnection("jdbc:oracle:thin:@localhost:1521:oracle","kdy","kdy");
	PreparedStatement stmt = conn.prepareStatement("insert into custom_01 values(?,?,?,?,?)");
	
	stmt.setString(1,request.getParameter("p_id"));
	stmt.setString(2,request.getParameter("p_pw"));
	stmt.setString(3,request.getParameter("c_name"));
	stmt.setString(4,request.getParameter("c_email"));
	stmt.setString(5,request.getParameter("c_tel"));
	
	stmt.executeUpdate();
%>
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title></title>
</head>
<body>
	<a href="member_insert.jsp">등록화면으로 돌아가기</a>
</body>
</html>