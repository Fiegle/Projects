package com.example.dm_app;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    private EditText edtEmail, edtPass;
    private Button btnLogin;
    private TextView txtRegister;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        edtEmail = findViewById(R.id.edtEmail);
        edtPass = findViewById(R.id.edtPass);
        btnLogin = findViewById(R.id.btnLogin);
        txtRegister = findViewById(R.id.txtRegister);

        txtRegister.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                Intent intent = new Intent(MainActivity.this, RegisterActivity.class);
                startActivity(intent);
                //Toast.makeText(MainActivity.this, "Register Clicked", Toast.LENGTH_SHORT).show();
            }
        });
        btnLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String Email, Pass;
                Email = edtEmail.getText().toString();
                Pass = edtPass.getText().toString();
                DataBaseHelper dataBaseHelper = new DataBaseHelper(MainActivity.this);
                boolean success = dataBaseHelper.isAuth(Email, Pass);
                if(success == true){
                    Toast.makeText(MainActivity.this, "Login Successful", Toast.LENGTH_SHORT).show();
                    Intent myIntent = new Intent(MainActivity.this, EmployeeActivity.class);
                    myIntent.putExtra("Email", Email);
                    myIntent.putExtra("NewEmail", Email);
                    myIntent.putExtra("Pass", Pass);
                    startActivity(myIntent);
                }else{
                    Toast.makeText(MainActivity.this, "Incorrect Email or Password", Toast.LENGTH_SHORT).show();
                }

            }
        });

    }

}