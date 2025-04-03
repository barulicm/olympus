function calculateScore(M0a,M1a,M2a)
{
  let score = 0;

  
  {
    
    
    let qa = M0a === 'y';
    
    
    
    
    score += (qa ? 20 : 0);
  }
  
  {
    
    
    
    
    let qa = parseInt(M1a);
    
    
    score += (qa * 5);
  }
  
  {
    
    
    
    let qa = M2a;
    
    
    
    score += (qa == "Option A" ? 10 : 0);
  }
  

  return score;
}
