<%@ page language="java" contentType="text/html; charset=EUC-KR"
    pageEncoding="EUC-KR"%>
<!DOCTYPE html>
<html>
<head>
<meta charset="EUC-KR">
<title>Insert title here</title>
</head>
<body>
	<h1>ȸ�� ���</h1>
	<form>
		<table border="1">
			<tr>
				<td>���̵�</td>
				<td><input type="text" name="p_id"></td>			
			</tr>
			<tr>
				<td>��й�ȣ</td>
				<td><input type="text" name="p_pw"></td>			
			</tr>
			<tr>
				<td>����</td>
				<td><input type="text" name="c_name"></td>			
			</tr>
			<tr>
				<td>�̸���</td>
				<td><input type="text" name="c_email"></td>			
			</tr>
			<tr>
				<td>����ó</td>
				<td><input type="text" name="c_tel"></td>			
			</tr>
			<tr>
				<td colspan="2">
					<button type="submit" formaction="add_apply.jsp">���</button>		
					<a href="find.jsp"><button type="button">��ȸ</button></a>		
					<button type="reset">����</button>							
				</td>
			</tr>
		</table>
	</form>
</body>
</html>














