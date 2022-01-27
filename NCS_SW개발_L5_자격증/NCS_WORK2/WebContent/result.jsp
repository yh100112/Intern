<%@ page language="java" contentType="text/html; charset=EUC-KR" pageEncoding="EUC-KR" import="kr.ac.kopo.*"%>
    
<%
	UserInfo user = new UserInfo("한국인","1004","2019-10-31");
	ProductInfo product = new ProductInfo();
	
	String code = request.getParameter("code");
	
	product.setCode(code);
	product.find();
%>
<!DOCTYPE html>
<html>
<head>
<meta charset="EUC-KR">
<title></title>
</head>
<body>
	<h1>제품코드 조회 결과</h1>
	<hr>
	<table border="1">
		<tr>
				<td colspan="2">제조사 조회 결과</td>
		</tr>
		<tr>
				<td>제품코드</td>
				<td><%= product.getCode() %></td>
		</tr>
		<tr>
				<td>제조사명</td>
				<td><%= product.getVendor() %></td>
		</tr>
		<tr>
				<td>조회일시</td>
				<td><%= product.getDate() %></td>
		</tr>
	</table>
	<hr>
	<div>
		작성자: <%= user.getName() %>비번:<%= user.getCode() %> 작성일:<%= user.getDate() %>
	</div>
</body>
</html>