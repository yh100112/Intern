<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8" import="java.sql.*"%>
<%
	Class.forName("oracle.jdbc.driver.OracleDriver");
	Connection conn = DriverManager.getConnection("jdbc:oracle:thin:@localhost:1521:oracle","kdy","kdy");
	Statement stmt = conn.createStatement();
	
	ResultSet rs = stmt.executeQuery("select * from custom_01 order by p_id");
%>
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title></title>
<style>
	table {text-align:center;}
</style>
</head>
<body>
	<h1>회원목록조회</h1>
	<hr>
	<table border="1">
		<thead>
			<tr>
				<th>회원아이디</th>
				<th>회원이름</th>
				<th>이메일</th>
				<th>연락처</th>
			</tr>
		</thead>
		<tbody>
			<% while(rs.next()){ %>
			<tr>
				<td><%= rs.getString(1) %></td>
				<td><%= rs.getString(3) %></td>
				<td><%= rs.getString(4) %></td>
				<td><%= rs.getString(5) %></td>
			</tr>
			<% } %>
		</tbody>
	</table>
	<hr>
	<h3>HRDKOREA Copyright&copy;2015 All rights reserved. Human Resources Development Service of Korea</h3>
</body>
</html>